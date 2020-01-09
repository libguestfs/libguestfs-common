/* libguestfs
 * Copyright (C) 2009-2019 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * Utility functions used by the library, tools and language bindings.
 *
 * These functions I<must not> call internal library functions
 * such as C<safe_*>, C<error> or C<perrorf>, or any C<guestfs_int_*>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

/* NB: MUST NOT include "guestfs-internal.h". */
#include "guestfs-stringlists-utils.h"

void
guestfs_int_free_string_list (char **argv)
{
  size_t i;

  if (argv == NULL)
    return;

  for (i = 0; argv[i] != NULL; ++i)
    free (argv[i]);
  free (argv);
}

size_t
guestfs_int_count_strings (char *const *argv)
{
  size_t r;

  for (r = 0; argv[r]; ++r)
    ;

  return r;
}

char **
guestfs_int_copy_string_list (char *const *argv)
{
  const size_t n = guestfs_int_count_strings (argv);
  size_t i, j;
  char **ret;

  ret = malloc ((n+1) * sizeof (char *));
  if (ret == NULL)
    return NULL;
  ret[n] = NULL;

  for (i = 0; i < n; ++i) {
    ret[i] = strdup (argv[i]);
    if (ret[i] == NULL) {
      for (j = 0; j < i; ++j)
        free (ret[j]);
      free (ret);
      return NULL;
    }
  }

  return ret;
}

/* Note that near-identical functions exist in the daemon. */
char *
guestfs_int_concat_strings (char *const *argv)
{
  return guestfs_int_join_strings ("", argv);
}

char *
guestfs_int_join_strings (const char *sep, char *const *argv)
{
  size_t i, len, seplen, rlen;
  char *r;

  seplen = strlen (sep);

  len = 0;
  for (i = 0; argv[i] != NULL; ++i) {
    if (i > 0)
      len += seplen;
    len += strlen (argv[i]);
  }
  len++; /* for final \0 */

  r = malloc (len);
  if (r == NULL)
    return NULL;

  rlen = 0;
  for (i = 0; argv[i] != NULL; ++i) {
    if (i > 0) {
      memcpy (&r[rlen], sep, seplen);
      rlen += seplen;
    }
    len = strlen (argv[i]);
    memcpy (&r[rlen], argv[i], len);
    rlen += len;
  }
  r[rlen] = '\0';

  return r;
}

/**
 * Split string at separator character C<sep>, returning the list of
 * strings.  Returns C<NULL> on memory allocation failure.
 *
 * Note (assuming C<sep> is C<:>):
 *
 * =over 4
 *
 * =item C<str == NULL>
 *
 * aborts
 *
 * =item C<str == "">
 *
 * returns C<[]>
 *
 * =item C<str == "abc">
 *
 * returns C<["abc"]>
 *
 * =item C<str == ":">
 *
 * returns C<["", ""]>
 *
 * =back
 */
char **
guestfs_int_split_string (char sep, const char *str)
{
  size_t i, n, c;
  const size_t len = strlen (str);
  char reject[2] = { sep, '\0' };
  char **ret;

  /* We have to handle the empty string case differently else the code
   * below will return [""].
   */
  if (str[0] == '\0') {
    ret = malloc (1 * sizeof (char *));
    if (!ret)
      return NULL;
    ret[0] = NULL;
    return ret;
  }

  for (n = i = 0; i < len; ++i)
    if (str[i] == sep)
      n++;

  /* We always return a list of length 1 + (# separator characters).
   * We also have to add a trailing NULL.
   */
  ret = malloc ((n+2) * sizeof (char *));
  if (!ret)
    return NULL;
  ret[n+1] = NULL;

  for (n = i = 0; i <= len; ++i, ++n) {
    c = strcspn (&str[i], reject);
    ret[n] = strndup (&str[i], c);
    if (ret[n] == NULL) {
      for (i = 0; i < n; ++i)
        free (ret[i]);
      free (ret);
      return NULL;
    }
    i += c;
    if (str[i] == '\0') /* end of string? */
      break;
  }

  return ret;
}
