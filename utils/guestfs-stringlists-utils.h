/* libguestfs
 * Copyright (C) 2013-2019 Red Hat Inc.
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

#ifndef GUESTFS_STRINGLISTS_UTILS_H_
#define GUESTFS_STRINGLISTS_UTILS_H_

/* stringlists-utils.c */
extern void guestfs_int_free_string_list (char **);
extern size_t guestfs_int_count_strings (char *const *)
  __attribute__((__nonnull__ (1)));
extern char *guestfs_int_concat_strings (char *const *);
extern char **guestfs_int_copy_string_list (char *const *);
extern char *guestfs_int_join_strings (const char *sep, char *const *);
extern char **guestfs_int_split_string (char sep, const char *);

#endif /* GUESTFS_STRINGLISTS_UTILS_H_ */
