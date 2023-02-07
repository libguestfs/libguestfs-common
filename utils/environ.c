/* nbdkit
 * Copyright (C) 2018-2023 Red Hat Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of Red Hat nor the names of its contributors may be
 * used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY RED HAT AND CONTRIBUTORS ''AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RED HAT OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "guestfs-stringlists-utils.h"
#include "guestfs-utils.h"

/* Copy an environ.  Also this allows you to add (key, value) pairs to
 * the environ through the varargs NULL-terminated list.  Returns NULL
 * if the copy or allocation failed.
 */
char **
guestfs_int_copy_environ (char **env, ...)
{
  char **ret = NULL, **new_ret;
  size_t i, len, nr_strings;
  char *s;
  va_list argp;
  const char *key, *value;

  /* Copy the existing keys into the new vector. */
  ret = guestfs_int_copy_string_list (env);
  if (ret == NULL)
    return NULL;
  nr_strings = guestfs_int_count_strings (ret);

  /* Add the new keys. */
  va_start (argp, env);
  while ((key = va_arg (argp, const char *)) != NULL) {
    value = va_arg (argp, const char *);
    if (asprintf (&s, "%s=%s", key, value) == -1) {
      va_end (argp);
      goto error;
    }

    /* Search for key in the existing environment.  It's O(n^2) ... */
    len = strlen (key);
    for (i = 0; ret[i] != NULL; ++i) {
      if (strncmp (key, ret[i], len) == 0 && ret[i][len] == '=') {
        /* Replace the existing key. */
        free (ret[i]);
        ret[i] = s;
        goto found;
      }
    }

    /* Else, append a new key to the list. */
    new_ret = realloc (ret, (nr_strings+2) * sizeof (char *));
    if (new_ret == NULL) {
      free (s);
      goto error;
    }
    ret = new_ret;
    ret[nr_strings+1] = NULL;
    ret[nr_strings] = s;
    nr_strings++;

  found: ;
  }
  va_end (argp);

  /* Return the list of strings. */
  return ret;

 error:
  for (i = 0; ret[i] != NULL; ++i)
    free (ret[i]);
  free (ret);
  return NULL;
}
