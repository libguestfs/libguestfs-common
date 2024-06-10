/* libguestfs generated file
 * WARNING: THIS FILE IS GENERATED FROM THE FOLLOWING FILES:
 *          generator/c.ml
 *          and from the code in the generator/ subdirectory.
 * ANY CHANGES YOU MAKE TO THIS FILE WILL BE LOST.
 *
 * Copyright (C) 2009-2023 Red Hat Inc.
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

#ifndef GUESTFS_INTERNAL_STRUCTS_PRINT_H_
#define GUESTFS_INTERNAL_STRUCTS_PRINT_H_

#include <stdio.h>

extern void guestfs_int_print_application_indent (struct guestfs_application *application, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_application2_indent (struct guestfs_application2 *application2, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_btrfsbalance_indent (struct guestfs_btrfsbalance *btrfsbalance, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_btrfsqgroup_indent (struct guestfs_btrfsqgroup *btrfsqgroup, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_btrfsscrub_indent (struct guestfs_btrfsscrub *btrfsscrub, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_btrfssubvolume_indent (struct guestfs_btrfssubvolume *btrfssubvolume, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_dirent_indent (struct guestfs_dirent *dirent, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_hivex_node_indent (struct guestfs_hivex_node *hivex_node, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_hivex_value_indent (struct guestfs_hivex_value *hivex_value, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_inotify_event_indent (struct guestfs_inotify_event *inotify_event, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_int_bool_indent (struct guestfs_int_bool *int_bool, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_isoinfo_indent (struct guestfs_isoinfo *isoinfo, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_lvm_lv_indent (struct guestfs_lvm_lv *lvm_lv, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_lvm_pv_indent (struct guestfs_lvm_pv *lvm_pv, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_lvm_vg_indent (struct guestfs_lvm_vg *lvm_vg, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_mdstat_indent (struct guestfs_mdstat *mdstat, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_partition_indent (struct guestfs_partition *partition, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_stat_indent (struct guestfs_stat *stat, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_statns_indent (struct guestfs_statns *statns, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_statvfs_indent (struct guestfs_statvfs *statvfs, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_tsk_dirent_indent (struct guestfs_tsk_dirent *tsk_dirent, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_utsname_indent (struct guestfs_utsname *utsname, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_version_indent (struct guestfs_version *version, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_xattr_indent (struct guestfs_xattr *xattr, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_xfsinfo_indent (struct guestfs_xfsinfo *xfsinfo, FILE *dest, const char *linesep, const char *indent);
extern void guestfs_int_print_yara_detection_indent (struct guestfs_yara_detection *yara_detection, FILE *dest, const char *linesep, const char *indent);

#if GUESTFS_PRIVATE

extern void guestfs_int_print_internal_mountable_indent (struct guestfs_internal_mountable *internal_mountable, FILE *dest, const char *linesep, const char *indent);

#endif /* End of GUESTFS_PRIVATE. */

#endif /* GUESTFS_INTERNAL_STRUCTS_PRINT_H_ */
