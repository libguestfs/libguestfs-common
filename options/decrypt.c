/* libguestfs - shared disk decryption
 * Copyright (C) 2010 Red Hat Inc.
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
 * This file implements the decryption of disk images, usually done
 * before mounting their partitions.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libintl.h>
#include <error.h>
#include <assert.h>
#include <errno.h>

#include "c-ctype.h"

#include "guestfs.h"

#include "options.h"

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

static void
append_char (size_t *idx, char *buffer, char c)
{
  /* bail out if the size of the string (including the terminating NUL, if any
   * cannot be expressed as a size_t
   */
  if (*idx == (size_t)-1)
    error (EXIT_FAILURE, 0, _("string size overflow"));

  /* if we're not just counting, then actually write the character */
  if (buffer != NULL)
    buffer[*idx] = c;

  /* advance */
  ++*idx;
}

/**
 * Make a LUKS map name from the partition or logical volume name, eg.
 * C<"/dev/vda2" =E<gt> "cryptvda2">, or C<"/dev/vg-ssd/lv-root7" =E<gt>
 * "cryptvgssdlvroot7">.  Note that, in logical volume device names,
 * c_isalnum() eliminates the "/" separator from between the VG and the LV, so
 * this mapping is not unique; but for our purposes, it will do.
 */
static char *
make_mapname (const char *device)
{
  bool strip_iprefix;
  static const char iprefix[] = "/dev/";
  char *mapname;
  enum { COUNT, WRITE, DONE } mode;

  strip_iprefix = STRPREFIX (device, iprefix);

  /* set to NULL in COUNT mode, flipped to non-NULL for WRITE mode */
  mapname = NULL;

  for (mode = COUNT; mode < DONE; ++mode) {
    size_t ipos;
    static const size_t iprefixlen = sizeof iprefix - 1;
    size_t opos;
    static const char oprefix[] = "crypt";
    static const size_t oprefixlen = sizeof oprefix - 1;
    char ichar;

    /* skip the input prefix, if any */
    ipos = strip_iprefix ? iprefixlen : 0;
    /* start producing characters after the output prefix */
    opos = oprefixlen;

    /* filter & copy */
    while ((ichar = device[ipos]) != '\0') {
      if (c_isalnum (ichar))
        append_char (&opos, mapname, ichar);
      ++ipos;
    }

    /* terminate */
    append_char (&opos, mapname, '\0');

    /* allocate the output buffer when flipping from COUNT to WRITE mode */
    if (mode == COUNT) {
      assert (opos >= sizeof oprefix);
      mapname = malloc (opos);
      if (mapname == NULL)
        error (EXIT_FAILURE, errno, "malloc");

      /* populate the output prefix -- note: not NUL-terminated yet */
      memcpy (mapname, oprefix, oprefixlen);
    }
  }

  return mapname;
}

static bool
decrypt_mountables (guestfs_h *g, const char * const *mountables,
                    struct key_store *ks)
{
  bool decrypted_some = false;
  const char * const *mnt_scan = mountables;
  const char *mountable;

  while ((mountable = *mnt_scan++) != NULL) {
    CLEANUP_FREE char *type = NULL;
    CLEANUP_FREE char *uuid = NULL;
    struct matching_key *keys;
    size_t nr_matches;
    CLEANUP_FREE char *mapname = NULL;
    size_t scan;

    type = guestfs_vfs_type (g, mountable);
    if (type == NULL)
      continue;

    /* "cryptsetup luksUUID" cannot read a UUID on Windows BitLocker disks
     * (unclear if this is a limitation of the format or cryptsetup).
     */
    if (STREQ (type, "crypto_LUKS")) {
      uuid = guestfs_luks_uuid (g, mountable);
    } else if (STRNEQ (type, "BitLocker"))
      continue;

    /* Grab the keys that we should try with this device, based on device name,
     * or UUID (if any).
     */
    keys = get_keys (ks, mountable, uuid, &nr_matches);
    assert (nr_matches > 0);

    /* Generate a node name for the plaintext (decrypted) device node. */
    if (uuid == NULL || asprintf (&mapname, "luks-%s", uuid) == -1)
      mapname = make_mapname (mountable);

    /* Try each key in turn. */
    for (scan = 0; scan < nr_matches; ++scan) {
      struct matching_key *key = keys + scan;
      int r;

      guestfs_push_error_handler (g, NULL, NULL);
      assert (key->clevis == (key->passphrase == NULL));
      if (key->clevis)
#ifdef GUESTFS_HAVE_CLEVIS_LUKS_UNLOCK
        r = guestfs_clevis_luks_unlock (g, mountable, mapname);
#else
        error (EXIT_FAILURE, 0,
               _("'clevis_luks_unlock', needed for decrypting %s, is "
                 "unavailable in this libguestfs version"), mountable);
#endif
      else
        r = guestfs_cryptsetup_open (g, mountable, key->passphrase, mapname,
                                     -1);
      guestfs_pop_error_handler (g);

      if (r == 0)
        break;
    }

    if (scan == nr_matches)
      error (EXIT_FAILURE, 0,
             _("could not find key to open LUKS encrypted %s.\n\n"
               "Try using --key on the command line.\n\n"
               "Original error: %s (%d)"),
             mountable, guestfs_last_error (g), guestfs_last_errno (g));

    free_keys (keys, nr_matches);
    decrypted_some = true;
  }

  return decrypted_some;
}

/**
 * Simple implementation of decryption: look for any encrypted
 * partitions and decrypt them, then rescan for VGs.
 */
void
inspect_do_decrypt (guestfs_h *g, struct key_store *ks)
{
  const char *lvm2_feature[] = { "lvm2", NULL };
  CLEANUP_FREE_STRING_LIST char **partitions = guestfs_list_partitions (g);
  bool need_rescan;

  if (partitions == NULL)
    exit (EXIT_FAILURE);

  need_rescan = decrypt_mountables (g, (const char * const *)partitions, ks);

  if (guestfs_feature_available (g, (char **) lvm2_feature) > 0) {
    CLEANUP_FREE_STRING_LIST char **lvs = NULL;

    if (need_rescan) {
      if (guestfs_lvm_scan (g, 1) == -1)
        exit (EXIT_FAILURE);
    }

    lvs = guestfs_lvs (g);
    if (lvs == NULL)
      exit (EXIT_FAILURE);
    decrypt_mountables (g, (const char * const *)lvs, ks);
  }
}
