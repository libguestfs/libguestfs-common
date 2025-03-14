/* libguestfs generated file
 * WARNING: THIS FILE IS GENERATED FROM THE FOLLOWING FILES:
 *          generator/c.ml
 *          and from the code in the generator/ subdirectory.
 * ANY CHANGES YOU MAKE TO THIS FILE WILL BE LOST.
 *
 * Copyright (C) 2009-2025 Red Hat Inc.
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

/* These CLEANUP_* macros automatically free the struct or struct list
 * pointed to by the local variable at the end of the current scope.
 */

#ifndef GUESTFS_STRUCTS_CLEANUPS_H_
#define GUESTFS_STRUCTS_CLEANUPS_H_

#define CLEANUP_FREE_INT_BOOL \
  __attribute__((cleanup(guestfs_int_cleanup_free_int_bool)))
#define CLEANUP_FREE_INT_BOOL_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_int_bool_list)))
#define CLEANUP_FREE_LVM_PV \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_pv)))
#define CLEANUP_FREE_LVM_PV_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_pv_list)))
#define CLEANUP_FREE_LVM_VG \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_vg)))
#define CLEANUP_FREE_LVM_VG_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_vg_list)))
#define CLEANUP_FREE_LVM_LV \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_lv)))
#define CLEANUP_FREE_LVM_LV_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_lvm_lv_list)))
#define CLEANUP_FREE_STAT \
  __attribute__((cleanup(guestfs_int_cleanup_free_stat)))
#define CLEANUP_FREE_STAT_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_stat_list)))
#define CLEANUP_FREE_STATNS \
  __attribute__((cleanup(guestfs_int_cleanup_free_statns)))
#define CLEANUP_FREE_STATNS_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_statns_list)))
#define CLEANUP_FREE_STATVFS \
  __attribute__((cleanup(guestfs_int_cleanup_free_statvfs)))
#define CLEANUP_FREE_STATVFS_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_statvfs_list)))
#define CLEANUP_FREE_DIRENT \
  __attribute__((cleanup(guestfs_int_cleanup_free_dirent)))
#define CLEANUP_FREE_DIRENT_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_dirent_list)))
#define CLEANUP_FREE_VERSION \
  __attribute__((cleanup(guestfs_int_cleanup_free_version)))
#define CLEANUP_FREE_VERSION_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_version_list)))
#define CLEANUP_FREE_XATTR \
  __attribute__((cleanup(guestfs_int_cleanup_free_xattr)))
#define CLEANUP_FREE_XATTR_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_xattr_list)))
#define CLEANUP_FREE_INOTIFY_EVENT \
  __attribute__((cleanup(guestfs_int_cleanup_free_inotify_event)))
#define CLEANUP_FREE_INOTIFY_EVENT_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_inotify_event_list)))
#define CLEANUP_FREE_PARTITION \
  __attribute__((cleanup(guestfs_int_cleanup_free_partition)))
#define CLEANUP_FREE_PARTITION_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_partition_list)))
#define CLEANUP_FREE_APPLICATION \
  __attribute__((cleanup(guestfs_int_cleanup_free_application)))
#define CLEANUP_FREE_APPLICATION_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_application_list)))
#define CLEANUP_FREE_APPLICATION2 \
  __attribute__((cleanup(guestfs_int_cleanup_free_application2)))
#define CLEANUP_FREE_APPLICATION2_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_application2_list)))
#define CLEANUP_FREE_ISOINFO \
  __attribute__((cleanup(guestfs_int_cleanup_free_isoinfo)))
#define CLEANUP_FREE_ISOINFO_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_isoinfo_list)))
#define CLEANUP_FREE_MDSTAT \
  __attribute__((cleanup(guestfs_int_cleanup_free_mdstat)))
#define CLEANUP_FREE_MDSTAT_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_mdstat_list)))
#define CLEANUP_FREE_BTRFSSUBVOLUME \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfssubvolume)))
#define CLEANUP_FREE_BTRFSSUBVOLUME_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfssubvolume_list)))
#define CLEANUP_FREE_BTRFSQGROUP \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsqgroup)))
#define CLEANUP_FREE_BTRFSQGROUP_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsqgroup_list)))
#define CLEANUP_FREE_BTRFSBALANCE \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsbalance)))
#define CLEANUP_FREE_BTRFSBALANCE_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsbalance_list)))
#define CLEANUP_FREE_BTRFSSCRUB \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsscrub)))
#define CLEANUP_FREE_BTRFSSCRUB_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_btrfsscrub_list)))
#define CLEANUP_FREE_XFSINFO \
  __attribute__((cleanup(guestfs_int_cleanup_free_xfsinfo)))
#define CLEANUP_FREE_XFSINFO_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_xfsinfo_list)))
#define CLEANUP_FREE_UTSNAME \
  __attribute__((cleanup(guestfs_int_cleanup_free_utsname)))
#define CLEANUP_FREE_UTSNAME_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_utsname_list)))
#define CLEANUP_FREE_HIVEX_NODE \
  __attribute__((cleanup(guestfs_int_cleanup_free_hivex_node)))
#define CLEANUP_FREE_HIVEX_NODE_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_hivex_node_list)))
#define CLEANUP_FREE_HIVEX_VALUE \
  __attribute__((cleanup(guestfs_int_cleanup_free_hivex_value)))
#define CLEANUP_FREE_HIVEX_VALUE_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_hivex_value_list)))
#define CLEANUP_FREE_INTERNAL_MOUNTABLE \
  __attribute__((cleanup(guestfs_int_cleanup_free_internal_mountable)))
#define CLEANUP_FREE_INTERNAL_MOUNTABLE_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_internal_mountable_list)))
#define CLEANUP_FREE_TSK_DIRENT \
  __attribute__((cleanup(guestfs_int_cleanup_free_tsk_dirent)))
#define CLEANUP_FREE_TSK_DIRENT_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_tsk_dirent_list)))
#define CLEANUP_FREE_YARA_DETECTION \
  __attribute__((cleanup(guestfs_int_cleanup_free_yara_detection)))
#define CLEANUP_FREE_YARA_DETECTION_LIST \
  __attribute__((cleanup(guestfs_int_cleanup_free_yara_detection_list)))

/* These functions are used internally by the CLEANUP_* macros.
 * Don't call them directly.
 */

extern void guestfs_int_cleanup_free_int_bool (void *ptr);
extern void guestfs_int_cleanup_free_int_bool_list (void *ptr);
extern void guestfs_int_cleanup_free_lvm_pv (void *ptr);
extern void guestfs_int_cleanup_free_lvm_pv_list (void *ptr);
extern void guestfs_int_cleanup_free_lvm_vg (void *ptr);
extern void guestfs_int_cleanup_free_lvm_vg_list (void *ptr);
extern void guestfs_int_cleanup_free_lvm_lv (void *ptr);
extern void guestfs_int_cleanup_free_lvm_lv_list (void *ptr);
extern void guestfs_int_cleanup_free_stat (void *ptr);
extern void guestfs_int_cleanup_free_stat_list (void *ptr);
extern void guestfs_int_cleanup_free_statns (void *ptr);
extern void guestfs_int_cleanup_free_statns_list (void *ptr);
extern void guestfs_int_cleanup_free_statvfs (void *ptr);
extern void guestfs_int_cleanup_free_statvfs_list (void *ptr);
extern void guestfs_int_cleanup_free_dirent (void *ptr);
extern void guestfs_int_cleanup_free_dirent_list (void *ptr);
extern void guestfs_int_cleanup_free_version (void *ptr);
extern void guestfs_int_cleanup_free_version_list (void *ptr);
extern void guestfs_int_cleanup_free_xattr (void *ptr);
extern void guestfs_int_cleanup_free_xattr_list (void *ptr);
extern void guestfs_int_cleanup_free_inotify_event (void *ptr);
extern void guestfs_int_cleanup_free_inotify_event_list (void *ptr);
extern void guestfs_int_cleanup_free_partition (void *ptr);
extern void guestfs_int_cleanup_free_partition_list (void *ptr);
extern void guestfs_int_cleanup_free_application (void *ptr);
extern void guestfs_int_cleanup_free_application_list (void *ptr);
extern void guestfs_int_cleanup_free_application2 (void *ptr);
extern void guestfs_int_cleanup_free_application2_list (void *ptr);
extern void guestfs_int_cleanup_free_isoinfo (void *ptr);
extern void guestfs_int_cleanup_free_isoinfo_list (void *ptr);
extern void guestfs_int_cleanup_free_mdstat (void *ptr);
extern void guestfs_int_cleanup_free_mdstat_list (void *ptr);
extern void guestfs_int_cleanup_free_btrfssubvolume (void *ptr);
extern void guestfs_int_cleanup_free_btrfssubvolume_list (void *ptr);
extern void guestfs_int_cleanup_free_btrfsqgroup (void *ptr);
extern void guestfs_int_cleanup_free_btrfsqgroup_list (void *ptr);
extern void guestfs_int_cleanup_free_btrfsbalance (void *ptr);
extern void guestfs_int_cleanup_free_btrfsbalance_list (void *ptr);
extern void guestfs_int_cleanup_free_btrfsscrub (void *ptr);
extern void guestfs_int_cleanup_free_btrfsscrub_list (void *ptr);
extern void guestfs_int_cleanup_free_xfsinfo (void *ptr);
extern void guestfs_int_cleanup_free_xfsinfo_list (void *ptr);
extern void guestfs_int_cleanup_free_utsname (void *ptr);
extern void guestfs_int_cleanup_free_utsname_list (void *ptr);
extern void guestfs_int_cleanup_free_hivex_node (void *ptr);
extern void guestfs_int_cleanup_free_hivex_node_list (void *ptr);
extern void guestfs_int_cleanup_free_hivex_value (void *ptr);
extern void guestfs_int_cleanup_free_hivex_value_list (void *ptr);
extern void guestfs_int_cleanup_free_internal_mountable (void *ptr);
extern void guestfs_int_cleanup_free_internal_mountable_list (void *ptr);
extern void guestfs_int_cleanup_free_tsk_dirent (void *ptr);
extern void guestfs_int_cleanup_free_tsk_dirent_list (void *ptr);
extern void guestfs_int_cleanup_free_yara_detection (void *ptr);
extern void guestfs_int_cleanup_free_yara_detection_list (void *ptr);

#endif /* GUESTFS_STRUCTS_CLEANUPS_H_ */
