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

#include "c-ctype.h"

#include "guestfs.h"

#include "options.h"

/**
 * Make a LUKS map name from the partition or logical volume name, eg.
 * C<"/dev/vda2" =E<gt> "cryptvda2">, or C<"/dev/vg-ssd/lv-root7" =E<gt>
 * "cryptvgssdlvroot7">.  Note that, in logical volume device names,
 * c_isalnum() eliminates the "/" separator from between the VG and the LV, so
 * this mapping is not unique; but for our purposes, it will do.
 */
static void
make_mapname (const char *device, char *mapname, size_t len)
{
  size_t i = 0;

  if (len < 6)
    abort ();
  strcpy (mapname, "crypt");
  mapname += 5;
  len -= 5;

  if (STRPREFIX (device, "/dev/"))
    i = 5;

  for (; device[i] != '\0' && len >= 1; ++i) {
    if (c_isalnum (device[i])) {
      *mapname++ = device[i];
      len--;
    }
  }

  *mapname = '\0';
}

static bool
decrypt_mountables (guestfs_h *g, const char * const *mountables,
                    struct key_store *ks, bool name_decrypted_by_uuid)
{
  bool decrypted_some = false;
  const char * const *mnt_scan = mountables;
  const char *mountable;

  while ((mountable = *mnt_scan++) != NULL) {
    CLEANUP_FREE char *type = NULL;
    CLEANUP_FREE char *uuid = NULL;
    CLEANUP_FREE_STRING_LIST char **keys = NULL;
    char mapname[512];
    const char * const *key_scan;
    const char *key;

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
    keys = get_keys (ks, mountable, uuid);
    assert (keys[0] != NULL);

    /* Generate a node name for the plaintext (decrypted) device node. */
    if (!name_decrypted_by_uuid || uuid == NULL ||
        snprintf (mapname, sizeof mapname, "luks-%s", uuid) < 0)
      make_mapname (mountable, mapname, sizeof mapname);

    /* Try each key in turn. */
    key_scan = (const char * const *)keys;
    while ((key = *key_scan++) != NULL) {
      int r;

      guestfs_push_error_handler (g, NULL, NULL);
      r = guestfs_cryptsetup_open (g, mountable, key, mapname, -1);
      guestfs_pop_error_handler (g);

      if (r == 0)
        break;
    }

    if (key == NULL)
      error (EXIT_FAILURE, 0,
             _("could not find key to open LUKS encrypted %s.\n\n"
               "Try using --key on the command line.\n\n"
               "Original error: %s (%d)"),
             mountable, guestfs_last_error (g), guestfs_last_errno (g));

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
  CLEANUP_FREE_STRING_LIST char **partitions = guestfs_list_partitions (g);
  CLEANUP_FREE_STRING_LIST char **lvs = NULL;
  bool need_rescan;

  if (partitions == NULL)
    exit (EXIT_FAILURE);

  need_rescan = decrypt_mountables (g, (const char * const *)partitions, ks,
                                    false);

  if (need_rescan) {
    if (guestfs_lvm_scan (g, 1) == -1)
      exit (EXIT_FAILURE);
  }

  lvs = guestfs_lvs (g);
  if (lvs == NULL)
    exit (EXIT_FAILURE);
  decrypt_mountables (g, (const char * const *)lvs, ks, true);
}
