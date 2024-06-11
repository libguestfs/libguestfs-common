/* Bindings for Perl-compatible Regular Expressions.
 * Copyright (C) 2017 Red Hat Inc.
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>

#include "cleanups.h"

#pragma GCC diagnostic ignored "-Wmissing-prototypes"

/* Replacement if caml_alloc_initialized_string is missing, added
 * to OCaml runtime in 2017.
 */
#ifndef HAVE_CAML_ALLOC_INITIALIZED_STRING
static inline value
caml_alloc_initialized_string (mlsize_t len, const char *p)
{
  value sv = caml_alloc_string (len);
  memcpy ((char *) String_val (sv), p, len);
  return sv;
}
#endif

/* Data on the most recent match is stored in this thread-local
 * variable.  It is freed either by the next call to PCRE.matches or
 * by (clean) thread exit.
 */
static pthread_key_t last_match;

struct last_match {
  char *subject;                /* subject string */
  pcre2_match_data *match_data; /* match offsets */
  int r;                        /* value returned by pcre2_match */
};

static void
free_last_match (struct last_match *data)
{
  if (data) {
    free (data->subject);
    pcre2_match_data_free (data->match_data);
    free (data);
  }
}

static void lm_init (void) __attribute__((constructor));
static void lm_free (void) __attribute__((destructor));

static void
lm_init (void)
{
  int err;

  err = pthread_key_create (&last_match, (void (*) (void *))free_last_match);
  if (err != 0) abort ();
}

static void
lm_free (void)
{
  pthread_key_delete (last_match);
}

/* Raises PCRE.error (errnum). */
static void
raise_pcre_error (int errnum)
{
  PCRE2_UCHAR err[256];
  value args[2];

  pcre2_get_error_message (errnum, err, sizeof err);

  args[0] = caml_copy_string ((char *) err);
  args[1] = Val_int (errnum);
  caml_raise_with_args (*caml_named_value ("PCRE.Error"), 2, args);
}

/* Raises PCRE.error with a non-library error.  The code field will be 0. */
static void
raise_pcre_other_error (const char *msg)
{
  value args[2];

  args[0] = caml_copy_string (msg);
  args[1] = Val_int (0);
  caml_raise_with_args (*caml_named_value ("PCRE.Error"), 2, args);
}

/* Wrap and unwrap pcre regular expression handles, with a finalizer. */
#define Regexp_val(rv) (*(pcre2_code **)Data_custom_val(rv))

static void
regexp_finalize (value rev)
{
  pcre2_code *re = Regexp_val (rev);
  if (re) pcre2_code_free (re);
}

static struct custom_operations custom_operations = {
  (char *) "pcre_custom_operations",
  regexp_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default,
  custom_compare_ext_default,
};

static value
Val_regexp (pcre2_code *re)
{
  CAMLparam0 ();
  CAMLlocal1 (rv);

  rv = caml_alloc_custom (&custom_operations, sizeof (pcre2_code *), 0, 1);
  Regexp_val (rv) = re;

  CAMLreturn (rv);
}

static int
is_Some_true (value v)
{
  return
    v != Val_int (0) /* !None */ &&
    Bool_val (Field (v, 0)) /* Some true */;
}

static int
Optint_val (value intv, int defval)
{
  if (intv == Val_int (0))      /* None */
    return defval;
  else                          /* Some int */
    return Int_val (Field (intv, 0));
}

value
guestfs_int_pcre_compile (value caselessv, value dotallv,
                          value extendedv, value multilinev,
                          value pattv)
{
  CAMLparam4 (caselessv, dotallv, extendedv, multilinev);
  CAMLxparam1 (pattv);
  const char *patt;
  int options = 0;
  pcre2_code *re;
  int errcode = 0;
  PCRE2_SIZE errnum;

  /* Flag parameters are all ‘bool option’, defaulting to false. */
  if (is_Some_true (caselessv))
    options |= PCRE2_CASELESS;
  if (is_Some_true (dotallv))
    options |= PCRE2_DOTALL;
  if (is_Some_true (extendedv))
    options |= PCRE2_EXTENDED;
  if (is_Some_true (multilinev))
    options |= PCRE2_MULTILINE;

  patt = String_val (pattv);

  re = pcre2_compile ((PCRE2_SPTR) patt, strlen (patt),
                      options, &errcode, &errnum, NULL);
  if (re == NULL)
    raise_pcre_error (errcode);

  CAMLreturn (Val_regexp (re));
}

value
guestfs_int_pcre_matches (value offsetv, value rev, value strv)
{
  CAMLparam3 (offsetv, rev, strv);
  pcre2_code *re = Regexp_val (rev);
  struct last_match *m, *oldm;
  size_t len = caml_string_length (strv);
  int r;

  m = calloc (1, sizeof *m);
  if (m == NULL)
    caml_raise_out_of_memory ();

  /* We will need the original subject string when fetching
   * substrings, so take a copy.
   */
  m->subject = malloc (len+1);
  if (m->subject == NULL) {
    free_last_match (m);
    caml_raise_out_of_memory ();
  }
  memcpy (m->subject, String_val (strv), len+1);

  /* Allocate the match_data. */
  m->match_data = pcre2_match_data_create_from_pattern (re, NULL);
  if (m->match_data == NULL) {
    free_last_match (m);
    caml_raise_out_of_memory ();
  }

  m->r = pcre2_match (re, (PCRE2_SPTR) m->subject, len,
                      Optint_val (offsetv, 0), 0,
                      m->match_data, NULL);
  if (m->r < 0 && m->r != PCRE2_ERROR_NOMATCH) {
    int ret = m->r;
    free_last_match (m);
    raise_pcre_error (ret);
  }

  /* This error would indicate that pcre_exec ran out of space in the
   * vector.  However if we are calculating the size of the vector
   * correctly above, then this should never happen.
   */
  assert (m->r != 0);

  r = m->r != PCRE2_ERROR_NOMATCH;

  /* Replace the old TLS match data, but only if we're going
   * to return a match.
   */
  if (r) {
    oldm = pthread_getspecific (last_match);
    free_last_match (oldm);
    pthread_setspecific (last_match, m);
  }
  else
    free_last_match (m);

  CAMLreturn (r ? Val_true : Val_false);
}

value
guestfs_int_pcre_sub (value nv)
{
  CAMLparam1 (nv);
  const int n = Int_val (nv);
  CAMLlocal1 (strv);
  const struct last_match *m = pthread_getspecific (last_match);
  PCRE2_SIZE len;
  int r;

  if (m == NULL)
    raise_pcre_other_error ("PCRE.sub called without calling PCRE.matches");

  if (n < 0)
    caml_invalid_argument ("PCRE.sub: n must be >= 0");

  r = pcre2_substring_length_bynumber (m->match_data, n, &len);
  if (r == PCRE2_ERROR_NOSUBSTRING || r == PCRE2_ERROR_UNSET)
    caml_raise_not_found ();
  if (r < 0)
    raise_pcre_error (r);

  strv = caml_alloc_string (len);

  /* This is fine.  OCaml allocates space for the trailing \0
   * and pcre expects that the buffer will be large enough to
   * store it.
   */
  len++;

  r = pcre2_substring_copy_bynumber (m->match_data, n,
                                     (PCRE2_UCHAR *) String_val (strv),
                                     &len);
  if (r < 0)
    raise_pcre_error (r);

  CAMLreturn (strv);
}

value
guestfs_int_pcre_subi (value nv)
{
  CAMLparam1 (nv);
  const int n = Int_val (nv);
  CAMLlocal1 (rv);
  const struct last_match *m = pthread_getspecific (last_match);
  PCRE2_SIZE *vec;

  if (m == NULL)
    raise_pcre_other_error ("PCRE.subi called without calling PCRE.matches");

  if (n < 0)
    caml_invalid_argument ("PCRE.subi: n must be >= 0");

  /* eg if there are 2 captures, m->r == 3, and valid values of n are
   * 0, 1 or 2.
   */
  if (n >= m->r)
    caml_raise_not_found ();

  vec = pcre2_get_ovector_pointer (m->match_data);

  rv = caml_alloc (2, 0);
  Store_field (rv, 0, Val_int (vec[n*2]));
  Store_field (rv, 1, Val_int (vec[n*2+1]));

  CAMLreturn (rv);
}
