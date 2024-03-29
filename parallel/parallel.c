/* virt-df
 * Copyright (C) 2013 Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * This file is used by C<virt-df> and some of the other tools when
 * they need to run multiple parallel libguestfs instances to operate
 * on a large number of libvirt domains efficiently.
 *
 * It implements a multithreaded work queue.  In addition it reorders
 * the output so the output still appears in the same order as the
 * input (ie. still ordered alphabetically).
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libintl.h>
#include <errno.h>
#include <error.h>

#include <pthread.h>

#ifdef HAVE_LIBVIRT
#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>
#endif

#include "ignore-value.h"
#include "getprogname.h"

#include "guestfs.h"
#include "guestfs-utils.h"
#include "options.h"
#include "domains.h"
#include "estimate-max-threads.h"
#include "parallel.h"

#if defined(HAVE_LIBVIRT)

/* Maximum number of threads we would ever run.  Note this should not
 * be > 20, unless libvirt is modified to increase the maximum number
 * of clients.
 */
#define MAX_THREADS 12

/* The worker threads take domains off the 'domains' global list until
 * 'next_domain_to_take' is 'nr_threads'.
 *
 * The worker threads retire domains in numerical order, using the
 * 'next_domain_to_retire' number.
 *
 * 'next_domain_to_take' is protected just by a mutex.
 * 'next_domain_to_retire' is protected by a mutex and condition.
 */
static size_t next_domain_to_take = 0;
static pthread_mutex_t take_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t next_domain_to_retire = 0;
static pthread_mutex_t retire_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t retire_cond = PTHREAD_COND_INITIALIZER;

static void thread_failure (const char *fn, int err);
static void *worker_thread (void *arg);

struct thread_data {
  size_t thread_num;            /* Thread number. */
  int trace, verbose;           /* Flags from the options_handle. */
  work_fn work;
  int r;                        /* Used to store the error status. */
};

/**
 * Run the threads and work through the global list of libvirt
 * domains.
 *
 * C<option_P> is whatever the user passed in the I<-P> option, or
 * C<0> if the user didn't use the I<-P> option (in which case the
 * number of threads is chosen heuristically).
 *
 * C<options_handle> (which may be C<NULL>) is the global guestfs
 * handle created by the options mini-library.
 *
 * The work function (C<work>) should do the work (inspecting the
 * domain, etc.)  on domain index C<i>.  However it I<must not> print
 * out any result directly.  Instead it prints anything it needs to
 * the supplied C<FILE *>.  The work function should return C<0> on
 * success or C<-1> on error.
 *
 * The C<start_threads> function returns C<0> if all work items
 * completed successfully, or C<-1> if there was an error.
 */
int
start_threads (size_t option_P, guestfs_h *options_handle, work_fn work)
{
  const int trace = options_handle ? guestfs_get_trace (options_handle) : 0;
  const int verbose = options_handle ? guestfs_get_verbose (options_handle) : 0;
  size_t i, nr_threads;
  int err, errors;
  void *status;
  CLEANUP_FREE struct thread_data *thread_data = NULL;
  CLEANUP_FREE pthread_t *threads = NULL;

  if (nr_domains == 0)          /* Nothing to do. */
    return 0;

  /* If the user selected the -P option, then we use up to that many threads. */
  if (option_P > 0)
    nr_threads = MIN (nr_domains, option_P);
  else
    nr_threads = MIN (nr_domains, MIN (MAX_THREADS, estimate_max_threads ()));

  if (verbose)
    fprintf (stderr, "parallel: creating %zu threads\n", nr_threads);

  thread_data = malloc (sizeof (struct thread_data) * nr_threads);
  threads = malloc (sizeof (pthread_t) * nr_threads);
  if (thread_data == NULL || threads == NULL)
    error (EXIT_FAILURE, errno, "malloc");

  for (i = 0; i < nr_threads; ++i) {
    thread_data[i].thread_num = i;
    thread_data[i].trace = trace;
    thread_data[i].verbose = verbose;
    thread_data[i].work = work;
  }

  /* Start the worker threads. */
  for (i = 0; i < nr_threads; ++i) {
    err = pthread_create (&threads[i], NULL, worker_thread, &thread_data[i]);
    if (err != 0)
      error (EXIT_FAILURE, err, "pthread_create [%zu]", i);
  }

  /* Wait for the threads to exit. */
  errors = 0;
  for (i = 0; i < nr_threads; ++i) {
    err = pthread_join (threads[i], &status);
    if (err != 0) {
      error (0, err, "pthread_join [%zu]", i);
      errors++;
    }
    if (*(int *)status == -1)
      errors++;
  }

  return errors == 0 ? 0 : -1;
}

static void *
worker_thread (void *thread_data_vp)
{
  struct thread_data *thread_data = thread_data_vp;

  thread_data->r = 0;

  if (thread_data->verbose)
    fprintf (stderr, "parallel: thread %zu: starting\n",
             thread_data->thread_num);

  while (1) {
    size_t i;               /* The current domain we're working on. */
    FILE *fp;
    CLEANUP_FREE char *output = NULL;
    size_t output_len = 0;
    guestfs_h *g;
    int err;
    char id[64];

    /* Take the next domain from the list. */
    if (thread_data->verbose)
      fprintf (stderr, "parallel: thread %zu: waiting to get work\n",
               thread_data->thread_num);

    err = pthread_mutex_lock (&take_mutex);
    if (err != 0) {
      thread_failure ("pthread_mutex_lock", err);
      thread_data->r = -1;
      return &thread_data->r;
    }
    i = next_domain_to_take++;
    err = pthread_mutex_unlock (&take_mutex);
    if (err != 0) {
      thread_failure ("pthread_mutex_unlock", err);
      thread_data->r = -1;
      return &thread_data->r;
    }

    if (i >= nr_domains)        /* Work finished. */
      break;

    if (thread_data->verbose)
      fprintf (stderr, "parallel: thread %zu: taking domain %zu\n",
               thread_data->thread_num, i);

    fp = open_memstream (&output, &output_len);
    if (fp == NULL) {
      perror ("open_memstream");
      thread_data->r = -1;
      return &thread_data->r;
    }

    /* Create a guestfs handle. */
    g = guestfs_create ();
    if (g == NULL) {
      perror ("guestfs_create");
      thread_data->r = -1;
      return &thread_data->r;
    }

    /* Set the handle identifier so we can tell threads apart. */
    snprintf (id, sizeof id, "thread_%zu_domain_%zu",
              thread_data->thread_num, i);
    guestfs_set_identifier (g, id);

    /* Copy some settings from the options guestfs handle. */
    guestfs_set_trace (g, thread_data->trace);
    guestfs_set_verbose (g, thread_data->verbose);

    /* Do work. */
    if (thread_data->work (g, i, fp) == -1) {
      thread_data->r = -1;

      if (thread_data->verbose)
        fprintf (stderr,
                 "parallel: thread %zu: work function returned an error\n",
                 thread_data->thread_num);
    }

    fclose (fp);
    guestfs_close (g);

    /* Retire this domain.  We have to retire domains in order, which
     * may mean waiting for another thread to finish here.
     */
    err = pthread_mutex_lock (&retire_mutex);
    if (err != 0) {
      thread_failure ("pthread_mutex_lock", err);
      thread_data->r = -1;
      return &thread_data->r;
    }

    if (thread_data->verbose)
      fprintf (stderr, "parallel: thread %zu: "
               "waiting to retire domain %zu "
               "(next domain to retire %zu)"
               "\n",
               thread_data->thread_num, i, next_domain_to_retire);

    while (next_domain_to_retire != i) {
      err = pthread_cond_wait (&retire_cond, &retire_mutex);
      if (err != 0) {
        thread_failure ("pthread_cond_wait", err);
        thread_data->r = -1;
        ignore_value (pthread_mutex_unlock (&retire_mutex));
        return &thread_data->r;
      }
    }

    if (thread_data->verbose)
      fprintf (stderr, "parallel: thread %zu: retiring domain %zu\n",
               thread_data->thread_num, i);

    /* Retire domain. */
    printf ("%s", output);

    /* Update next_domain_to_retire and tell other threads. */
    if (thread_data->verbose)
      fprintf (stderr, "parallel: thread %zu: "
               "next domain to retire %zu -> %zu\n",
               thread_data->thread_num, next_domain_to_retire, i+1);

    next_domain_to_retire = i+1;
    pthread_cond_broadcast (&retire_cond);
    err = pthread_mutex_unlock (&retire_mutex);
    if (err != 0) {
      thread_failure ("pthread_mutex_unlock", err);
      thread_data->r = -1;
      return &thread_data->r;
    }
  }

  if (thread_data->verbose)
    fprintf (stderr, "parallel: thread %zu: exiting (r = %d)\n",
             thread_data->thread_num, thread_data->r);

  return &thread_data->r;
}

static void
thread_failure (const char *fn, int err)
{
  fprintf (stderr, "%s: %s: %s\n",
           getprogname (), fn, strerror (err));
}

#endif /* HAVE_LIBVIRT */
