/* libguestfs - guestfish and guestmount shared option parsing
 * Copyright (C) 2010-2023 Red Hat Inc.
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
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <libintl.h>
#include <errno.h>
#include <error.h>
#include <assert.h>

#include "guestfs.h"

#include "options.h"

/**
 * Read a passphrase ('Key') from F</dev/tty> with echo off.
 *
 * The caller (F<fish/cmds.c>) will call free on the string
 * afterwards.  Based on the code in cryptsetup file F<lib/utils.c>.
 */
char *
read_key (const char *param)
{
  FILE *infp, *outfp;
  struct termios orig, temp;
  char *ret = NULL;
  int tty;
  int tcset = 0;
  size_t allocsize = 0;
  ssize_t len;

  /* Read and write to /dev/tty if available. */
  if (keys_from_stdin ||
      (infp = outfp = fopen ("/dev/tty", "w+")) == NULL) {
    infp = stdin;
    outfp = stdout;
  }

  /* Print the prompt and set no echo. */
  tty = isatty (fileno (infp));
  if (tty) {
    fprintf (outfp, _("Enter key or passphrase (\"%s\"): "), param);

    if (!echo_keys) {
      if (tcgetattr (fileno (infp), &orig) == -1) {
        perror ("tcgetattr");
        goto error;
      }
      memcpy (&temp, &orig, sizeof temp);
      temp.c_lflag &= ~ECHO;

      tcsetattr (fileno (infp), TCSAFLUSH, &temp);
      tcset = 1;
    }
  }

  len = getline (&ret, &allocsize, infp);
  if (len == -1) {
    perror ("getline");
    ret = NULL;
    goto error;
  }

  /* Remove the terminating \n if there is one. */
  if (len > 0 && ret[len-1] == '\n')
    ret[len-1] = '\0';

 error:
  /* Restore echo, close file descriptor. */
  if (tty && tcset) {
    printf ("\n");
    tcsetattr (fileno (infp), TCSAFLUSH, &orig);
  }

  if (infp != stdin)
    fclose (infp); /* outfp == infp, so this is closed also */

  return ret;
}

static char *
read_first_line_from_file (const char *filename)
{
  CLEANUP_FCLOSE FILE *fp = NULL;
  char *ret = NULL;
  size_t allocsize = 0;
  ssize_t len;

  fp = fopen (filename, "r");
  if (!fp)
    error (EXIT_FAILURE, errno, "fopen: %s", filename);

  len = getline (&ret, &allocsize, fp);
  if (len == -1)
    error (EXIT_FAILURE, errno, "getline: %s", filename);

  /* Remove the terminating \n if there is one. */
  if (len > 0 && ret[len-1] == '\n')
    ret[len-1] = '\0';

  return ret;
}

/* Return the key(s) matching this particular device from the
 * keystore.  There may be multiple.  If none are read from the
 * keystore, ask the user.
 */
struct matching_key *
get_keys (struct key_store *ks, const char *device, const char *uuid,
          size_t *nr_matches)
{
  size_t i, nmemb;
  struct matching_key *r, *match;
  char *s;

  /* We know the returned list must have at least one element and not
   * more than ks->nr_keys.
   */
  nmemb = 1;
  if (ks && ks->nr_keys > nmemb)
    nmemb = ks->nr_keys;

  if (nmemb > (size_t)-1 / sizeof *r)
    error (EXIT_FAILURE, 0, _("size_t overflow"));

  r = malloc (nmemb * sizeof *r);
  if (r == NULL)
    error (EXIT_FAILURE, errno, "malloc");

  match = r;

  if (ks) {
    for (i = 0; i < ks->nr_keys; ++i) {
      struct key_store_key *key = &ks->keys[i];

      if (STRNEQ (key->id, device) && (!uuid || STRNEQ (key->id, uuid)))
        continue;

      switch (key->type) {
      case key_string:
        s = strdup (key->string.s);
        if (!s)
          error (EXIT_FAILURE, errno, "strdup");
        match->clevis = false;
        match->passphrase = s;
        ++match;
        break;
      case key_file:
        s = read_first_line_from_file (key->file.name);
        match->clevis = false;
        match->passphrase = s;
        ++match;
        break;
      case key_clevis:
        match->clevis = true;
        match->passphrase = NULL;
        ++match;
        break;
      }
    }
  }

  if (match == r) {
    /* Key not found in the key store, ask the user for it. */
    s = read_key (device);
    if (!s)
      error (EXIT_FAILURE, 0, _("could not read key from user"));
    match->clevis = false;
    match->passphrase = s;
    ++match;
  }

  *nr_matches = (size_t)(match - r);
  return r;
}

void
free_keys (struct matching_key *keys, size_t nr_matches)
{
  size_t i;

  for (i = 0; i < nr_matches; ++i) {
    struct matching_key *key = keys + i;

    assert (key->clevis == (key->passphrase == NULL));
    if (!key->clevis)
      free (key->passphrase);
  }
  free (keys);
}

struct key_store *
key_store_add_from_selector (struct key_store *ks, const char *selector)
{
  CLEANUP_FREE_STRING_LIST char **fields = NULL;
  size_t field_count;
  struct key_store_key key;

  fields = guestfs_int_split_string (':', selector);
  if (!fields)
    error (EXIT_FAILURE, errno, "guestfs_int_split_string");
  field_count = guestfs_int_count_strings (fields);

  /* field#0: ID */
  if (field_count < 1)
    error (EXIT_FAILURE, 0, _("selector '%s': missing ID"), selector);
  key.id = strdup (fields[0]);
  if (!key.id)
    error (EXIT_FAILURE, errno, "strdup");

  /* field#1...: TYPE, and TYPE-specific properties */
  if (field_count < 2)
    error (EXIT_FAILURE, 0, _("selector '%s': missing TYPE"), selector);

  if (STREQ (fields[1], "key")) {
    key.type = key_string;
    if (field_count != 3)
      error (EXIT_FAILURE, 0,
             _("selector '%s': missing KEY_STRING, or too many fields"),
             selector);
    key.string.s = strdup (fields[2]);
    if (!key.string.s)
      error (EXIT_FAILURE, errno, "strdup");
  } else if (STREQ (fields[1], "file")) {
    key.type = key_file;
    if (field_count != 3)
      error (EXIT_FAILURE, 0,
             _("selector '%s': missing FILENAME, or too many fields"),
             selector);
    key.file.name = strdup (fields[2]);
    if (!key.file.name)
      error (EXIT_FAILURE, errno, "strdup");
  } else if (STREQ (fields[1], "clevis")) {
    key.type = key_clevis;
    if (field_count != 2)
      error (EXIT_FAILURE, 0, _("selector '%s': too many fields"), selector);
  } else
    error (EXIT_FAILURE, 0, _("selector '%s': invalid TYPE"), selector);

  return key_store_import_key (ks, &key);
}

struct key_store *
key_store_import_key (struct key_store *ks, const struct key_store_key *key)
{
  struct key_store_key *new_keys;

  if (!ks) {
    ks = calloc (1, sizeof (*ks));
    if (!ks)
      error (EXIT_FAILURE, errno, "strdup");
  }
  assert (ks != NULL);

  new_keys = realloc (ks->keys,
                      (ks->nr_keys + 1) * sizeof (struct key_store_key));
  if (!new_keys)
    error (EXIT_FAILURE, errno, "realloc");

  ks->keys = new_keys;
  ks->keys[ks->nr_keys] = *key;
  ++ks->nr_keys;

  return ks;
}

bool
key_store_requires_network (const struct key_store *ks)
{
  size_t i;

  if (ks == NULL)
    return false;

  for (i = 0; i < ks->nr_keys; ++i)
    if (ks->keys[i].type == key_clevis)
      return true;

  return false;
}

void
free_key_store (struct key_store *ks)
{
  size_t i;

  if (!ks)
    return;

  for (i = 0; i < ks->nr_keys; ++i) {
    struct key_store_key *key = &ks->keys[i];

    switch (key->type) {
    case key_string:
      free (key->string.s);
      break;
    case key_file:
      free (key->file.name);
      break;
    case key_clevis:
      /* nothing */
      break;
    }
    free (key->id);
  }

  free (ks->keys);
  free (ks);
}
