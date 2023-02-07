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

#include <config.h>

#include <inttypes.h>

#include "c-ctype.h"

#include "guestfs.h"
#include "structs-print.h"

void
guestfs_int_print_application_indent (struct guestfs_application *application, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sapp_name: %s%s", indent, application->app_name, linesep);
  fprintf (dest, "%sapp_display_name: %s%s", indent, application->app_display_name, linesep);
  fprintf (dest, "%sapp_epoch: %" PRIi32 "%s", indent, application->app_epoch, linesep);
  fprintf (dest, "%sapp_version: %s%s", indent, application->app_version, linesep);
  fprintf (dest, "%sapp_release: %s%s", indent, application->app_release, linesep);
  fprintf (dest, "%sapp_install_path: %s%s", indent, application->app_install_path, linesep);
  fprintf (dest, "%sapp_trans_path: %s%s", indent, application->app_trans_path, linesep);
  fprintf (dest, "%sapp_publisher: %s%s", indent, application->app_publisher, linesep);
  fprintf (dest, "%sapp_url: %s%s", indent, application->app_url, linesep);
  fprintf (dest, "%sapp_source_package: %s%s", indent, application->app_source_package, linesep);
  fprintf (dest, "%sapp_summary: %s%s", indent, application->app_summary, linesep);
  fprintf (dest, "%sapp_description: %s%s", indent, application->app_description, linesep);
}

void
guestfs_int_print_application2_indent (struct guestfs_application2 *application2, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sapp2_name: %s%s", indent, application2->app2_name, linesep);
  fprintf (dest, "%sapp2_display_name: %s%s", indent, application2->app2_display_name, linesep);
  fprintf (dest, "%sapp2_epoch: %" PRIi32 "%s", indent, application2->app2_epoch, linesep);
  fprintf (dest, "%sapp2_version: %s%s", indent, application2->app2_version, linesep);
  fprintf (dest, "%sapp2_release: %s%s", indent, application2->app2_release, linesep);
  fprintf (dest, "%sapp2_arch: %s%s", indent, application2->app2_arch, linesep);
  fprintf (dest, "%sapp2_install_path: %s%s", indent, application2->app2_install_path, linesep);
  fprintf (dest, "%sapp2_trans_path: %s%s", indent, application2->app2_trans_path, linesep);
  fprintf (dest, "%sapp2_publisher: %s%s", indent, application2->app2_publisher, linesep);
  fprintf (dest, "%sapp2_url: %s%s", indent, application2->app2_url, linesep);
  fprintf (dest, "%sapp2_source_package: %s%s", indent, application2->app2_source_package, linesep);
  fprintf (dest, "%sapp2_summary: %s%s", indent, application2->app2_summary, linesep);
  fprintf (dest, "%sapp2_description: %s%s", indent, application2->app2_description, linesep);
  fprintf (dest, "%sapp2_spare1: %s%s", indent, application2->app2_spare1, linesep);
  fprintf (dest, "%sapp2_spare2: %s%s", indent, application2->app2_spare2, linesep);
  fprintf (dest, "%sapp2_spare3: %s%s", indent, application2->app2_spare3, linesep);
  fprintf (dest, "%sapp2_spare4: %s%s", indent, application2->app2_spare4, linesep);
}

void
guestfs_int_print_btrfsbalance_indent (struct guestfs_btrfsbalance *btrfsbalance, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sbtrfsbalance_status: %s%s", indent, btrfsbalance->btrfsbalance_status, linesep);
  fprintf (dest, "%sbtrfsbalance_total: %" PRIu64 "%s", indent, btrfsbalance->btrfsbalance_total, linesep);
  fprintf (dest, "%sbtrfsbalance_balanced: %" PRIu64 "%s", indent, btrfsbalance->btrfsbalance_balanced, linesep);
  fprintf (dest, "%sbtrfsbalance_considered: %" PRIu64 "%s", indent, btrfsbalance->btrfsbalance_considered, linesep);
  fprintf (dest, "%sbtrfsbalance_left: %" PRIu64 "%s", indent, btrfsbalance->btrfsbalance_left, linesep);
}

void
guestfs_int_print_btrfsqgroup_indent (struct guestfs_btrfsqgroup *btrfsqgroup, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sbtrfsqgroup_id: %s%s", indent, btrfsqgroup->btrfsqgroup_id, linesep);
  fprintf (dest, "%sbtrfsqgroup_rfer: %" PRIu64 "%s", indent, btrfsqgroup->btrfsqgroup_rfer, linesep);
  fprintf (dest, "%sbtrfsqgroup_excl: %" PRIu64 "%s", indent, btrfsqgroup->btrfsqgroup_excl, linesep);
}

void
guestfs_int_print_btrfsscrub_indent (struct guestfs_btrfsscrub *btrfsscrub, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sbtrfsscrub_data_extents_scrubbed: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_data_extents_scrubbed, linesep);
  fprintf (dest, "%sbtrfsscrub_tree_extents_scrubbed: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_tree_extents_scrubbed, linesep);
  fprintf (dest, "%sbtrfsscrub_data_bytes_scrubbed: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_data_bytes_scrubbed, linesep);
  fprintf (dest, "%sbtrfsscrub_tree_bytes_scrubbed: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_tree_bytes_scrubbed, linesep);
  fprintf (dest, "%sbtrfsscrub_read_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_read_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_csum_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_csum_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_verify_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_verify_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_no_csum: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_no_csum, linesep);
  fprintf (dest, "%sbtrfsscrub_csum_discards: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_csum_discards, linesep);
  fprintf (dest, "%sbtrfsscrub_super_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_super_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_malloc_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_malloc_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_uncorrectable_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_uncorrectable_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_unverified_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_unverified_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_corrected_errors: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_corrected_errors, linesep);
  fprintf (dest, "%sbtrfsscrub_last_physical: %" PRIu64 "%s", indent, btrfsscrub->btrfsscrub_last_physical, linesep);
}

void
guestfs_int_print_btrfssubvolume_indent (struct guestfs_btrfssubvolume *btrfssubvolume, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sbtrfssubvolume_id: %" PRIu64 "%s", indent, btrfssubvolume->btrfssubvolume_id, linesep);
  fprintf (dest, "%sbtrfssubvolume_top_level_id: %" PRIu64 "%s", indent, btrfssubvolume->btrfssubvolume_top_level_id, linesep);
  fprintf (dest, "%sbtrfssubvolume_path: %s%s", indent, btrfssubvolume->btrfssubvolume_path, linesep);
}

void
guestfs_int_print_dirent_indent (struct guestfs_dirent *dirent, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sino: %" PRIi64 "%s", indent, dirent->ino, linesep);
  fprintf (dest, "%sftyp: %c%s", indent, dirent->ftyp, linesep);
  fprintf (dest, "%sname: %s%s", indent, dirent->name, linesep);
}

void
guestfs_int_print_hivex_node_indent (struct guestfs_hivex_node *hivex_node, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%shivex_node_h: %" PRIi64 "%s", indent, hivex_node->hivex_node_h, linesep);
}

void
guestfs_int_print_hivex_value_indent (struct guestfs_hivex_value *hivex_value, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%shivex_value_h: %" PRIi64 "%s", indent, hivex_value->hivex_value_h, linesep);
}

void
guestfs_int_print_inotify_event_indent (struct guestfs_inotify_event *inotify_event, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sin_wd: %" PRIi64 "%s", indent, inotify_event->in_wd, linesep);
  fprintf (dest, "%sin_mask: %" PRIu32 "%s", indent, inotify_event->in_mask, linesep);
  fprintf (dest, "%sin_cookie: %" PRIu32 "%s", indent, inotify_event->in_cookie, linesep);
  fprintf (dest, "%sin_name: %s%s", indent, inotify_event->in_name, linesep);
}

void
guestfs_int_print_int_bool_indent (struct guestfs_int_bool *int_bool, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%si: %" PRIi32 "%s", indent, int_bool->i, linesep);
  fprintf (dest, "%sb: %" PRIi32 "%s", indent, int_bool->b, linesep);
}

void
guestfs_int_print_isoinfo_indent (struct guestfs_isoinfo *isoinfo, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%siso_system_id: %s%s", indent, isoinfo->iso_system_id, linesep);
  fprintf (dest, "%siso_volume_id: %s%s", indent, isoinfo->iso_volume_id, linesep);
  fprintf (dest, "%siso_volume_space_size: %" PRIu32 "%s", indent, isoinfo->iso_volume_space_size, linesep);
  fprintf (dest, "%siso_volume_set_size: %" PRIu32 "%s", indent, isoinfo->iso_volume_set_size, linesep);
  fprintf (dest, "%siso_volume_sequence_number: %" PRIu32 "%s", indent, isoinfo->iso_volume_sequence_number, linesep);
  fprintf (dest, "%siso_logical_block_size: %" PRIu32 "%s", indent, isoinfo->iso_logical_block_size, linesep);
  fprintf (dest, "%siso_volume_set_id: %s%s", indent, isoinfo->iso_volume_set_id, linesep);
  fprintf (dest, "%siso_publisher_id: %s%s", indent, isoinfo->iso_publisher_id, linesep);
  fprintf (dest, "%siso_data_preparer_id: %s%s", indent, isoinfo->iso_data_preparer_id, linesep);
  fprintf (dest, "%siso_application_id: %s%s", indent, isoinfo->iso_application_id, linesep);
  fprintf (dest, "%siso_copyright_file_id: %s%s", indent, isoinfo->iso_copyright_file_id, linesep);
  fprintf (dest, "%siso_abstract_file_id: %s%s", indent, isoinfo->iso_abstract_file_id, linesep);
  fprintf (dest, "%siso_bibliographic_file_id: %s%s", indent, isoinfo->iso_bibliographic_file_id, linesep);
  fprintf (dest, "%siso_volume_creation_t: %" PRIi64 "%s", indent, isoinfo->iso_volume_creation_t, linesep);
  fprintf (dest, "%siso_volume_modification_t: %" PRIi64 "%s", indent, isoinfo->iso_volume_modification_t, linesep);
  fprintf (dest, "%siso_volume_expiration_t: %" PRIi64 "%s", indent, isoinfo->iso_volume_expiration_t, linesep);
  fprintf (dest, "%siso_volume_effective_t: %" PRIi64 "%s", indent, isoinfo->iso_volume_effective_t, linesep);
}

void
guestfs_int_print_lvm_lv_indent (struct guestfs_lvm_lv *lvm_lv, FILE *dest, const char *linesep, const char *indent)
{
  size_t i;

  fprintf (dest, "%slv_name: %s%s", indent, lvm_lv->lv_name, linesep);
  fprintf (dest, "%slv_uuid: ", indent);
  for (i = 0; i < 32; ++i)
    fprintf (dest, "%c", lvm_lv->lv_uuid[i]);
  fprintf (dest, "%s", linesep);
  fprintf (dest, "%slv_attr: %s%s", indent, lvm_lv->lv_attr, linesep);
  fprintf (dest, "%slv_major: %" PRIi64 "%s", indent, lvm_lv->lv_major, linesep);
  fprintf (dest, "%slv_minor: %" PRIi64 "%s", indent, lvm_lv->lv_minor, linesep);
  fprintf (dest, "%slv_kernel_major: %" PRIi64 "%s", indent, lvm_lv->lv_kernel_major, linesep);
  fprintf (dest, "%slv_kernel_minor: %" PRIi64 "%s", indent, lvm_lv->lv_kernel_minor, linesep);
  fprintf (dest, "%slv_size: %" PRIu64 "%s", indent, lvm_lv->lv_size, linesep);
  fprintf (dest, "%sseg_count: %" PRIi64 "%s", indent, lvm_lv->seg_count, linesep);
  fprintf (dest, "%sorigin: %s%s", indent, lvm_lv->origin, linesep);
  if (lvm_lv->snap_percent >= 0)
    fprintf (dest, "%ssnap_percent: %g %%%s", indent, (double) lvm_lv->snap_percent, linesep);
  else
    fprintf (dest, "%ssnap_percent: %s", indent, linesep);
  if (lvm_lv->copy_percent >= 0)
    fprintf (dest, "%scopy_percent: %g %%%s", indent, (double) lvm_lv->copy_percent, linesep);
  else
    fprintf (dest, "%scopy_percent: %s", indent, linesep);
  fprintf (dest, "%smove_pv: %s%s", indent, lvm_lv->move_pv, linesep);
  fprintf (dest, "%slv_tags: %s%s", indent, lvm_lv->lv_tags, linesep);
  fprintf (dest, "%smirror_log: %s%s", indent, lvm_lv->mirror_log, linesep);
  fprintf (dest, "%smodules: %s%s", indent, lvm_lv->modules, linesep);
}

void
guestfs_int_print_lvm_pv_indent (struct guestfs_lvm_pv *lvm_pv, FILE *dest, const char *linesep, const char *indent)
{
  size_t i;

  fprintf (dest, "%spv_name: %s%s", indent, lvm_pv->pv_name, linesep);
  fprintf (dest, "%spv_uuid: ", indent);
  for (i = 0; i < 32; ++i)
    fprintf (dest, "%c", lvm_pv->pv_uuid[i]);
  fprintf (dest, "%s", linesep);
  fprintf (dest, "%spv_fmt: %s%s", indent, lvm_pv->pv_fmt, linesep);
  fprintf (dest, "%spv_size: %" PRIu64 "%s", indent, lvm_pv->pv_size, linesep);
  fprintf (dest, "%sdev_size: %" PRIu64 "%s", indent, lvm_pv->dev_size, linesep);
  fprintf (dest, "%spv_free: %" PRIu64 "%s", indent, lvm_pv->pv_free, linesep);
  fprintf (dest, "%spv_used: %" PRIu64 "%s", indent, lvm_pv->pv_used, linesep);
  fprintf (dest, "%spv_attr: %s%s", indent, lvm_pv->pv_attr, linesep);
  fprintf (dest, "%spv_pe_count: %" PRIi64 "%s", indent, lvm_pv->pv_pe_count, linesep);
  fprintf (dest, "%spv_pe_alloc_count: %" PRIi64 "%s", indent, lvm_pv->pv_pe_alloc_count, linesep);
  fprintf (dest, "%spv_tags: %s%s", indent, lvm_pv->pv_tags, linesep);
  fprintf (dest, "%spe_start: %" PRIu64 "%s", indent, lvm_pv->pe_start, linesep);
  fprintf (dest, "%spv_mda_count: %" PRIi64 "%s", indent, lvm_pv->pv_mda_count, linesep);
  fprintf (dest, "%spv_mda_free: %" PRIu64 "%s", indent, lvm_pv->pv_mda_free, linesep);
}

void
guestfs_int_print_lvm_vg_indent (struct guestfs_lvm_vg *lvm_vg, FILE *dest, const char *linesep, const char *indent)
{
  size_t i;

  fprintf (dest, "%svg_name: %s%s", indent, lvm_vg->vg_name, linesep);
  fprintf (dest, "%svg_uuid: ", indent);
  for (i = 0; i < 32; ++i)
    fprintf (dest, "%c", lvm_vg->vg_uuid[i]);
  fprintf (dest, "%s", linesep);
  fprintf (dest, "%svg_fmt: %s%s", indent, lvm_vg->vg_fmt, linesep);
  fprintf (dest, "%svg_attr: %s%s", indent, lvm_vg->vg_attr, linesep);
  fprintf (dest, "%svg_size: %" PRIu64 "%s", indent, lvm_vg->vg_size, linesep);
  fprintf (dest, "%svg_free: %" PRIu64 "%s", indent, lvm_vg->vg_free, linesep);
  fprintf (dest, "%svg_sysid: %s%s", indent, lvm_vg->vg_sysid, linesep);
  fprintf (dest, "%svg_extent_size: %" PRIu64 "%s", indent, lvm_vg->vg_extent_size, linesep);
  fprintf (dest, "%svg_extent_count: %" PRIi64 "%s", indent, lvm_vg->vg_extent_count, linesep);
  fprintf (dest, "%svg_free_count: %" PRIi64 "%s", indent, lvm_vg->vg_free_count, linesep);
  fprintf (dest, "%smax_lv: %" PRIi64 "%s", indent, lvm_vg->max_lv, linesep);
  fprintf (dest, "%smax_pv: %" PRIi64 "%s", indent, lvm_vg->max_pv, linesep);
  fprintf (dest, "%spv_count: %" PRIi64 "%s", indent, lvm_vg->pv_count, linesep);
  fprintf (dest, "%slv_count: %" PRIi64 "%s", indent, lvm_vg->lv_count, linesep);
  fprintf (dest, "%ssnap_count: %" PRIi64 "%s", indent, lvm_vg->snap_count, linesep);
  fprintf (dest, "%svg_seqno: %" PRIi64 "%s", indent, lvm_vg->vg_seqno, linesep);
  fprintf (dest, "%svg_tags: %s%s", indent, lvm_vg->vg_tags, linesep);
  fprintf (dest, "%svg_mda_count: %" PRIi64 "%s", indent, lvm_vg->vg_mda_count, linesep);
  fprintf (dest, "%svg_mda_free: %" PRIu64 "%s", indent, lvm_vg->vg_mda_free, linesep);
}

void
guestfs_int_print_mdstat_indent (struct guestfs_mdstat *mdstat, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%smdstat_device: %s%s", indent, mdstat->mdstat_device, linesep);
  fprintf (dest, "%smdstat_index: %" PRIi32 "%s", indent, mdstat->mdstat_index, linesep);
  fprintf (dest, "%smdstat_flags: %s%s", indent, mdstat->mdstat_flags, linesep);
}

void
guestfs_int_print_partition_indent (struct guestfs_partition *partition, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%spart_num: %" PRIi32 "%s", indent, partition->part_num, linesep);
  fprintf (dest, "%spart_start: %" PRIu64 "%s", indent, partition->part_start, linesep);
  fprintf (dest, "%spart_end: %" PRIu64 "%s", indent, partition->part_end, linesep);
  fprintf (dest, "%spart_size: %" PRIu64 "%s", indent, partition->part_size, linesep);
}

void
guestfs_int_print_stat_indent (struct guestfs_stat *stat, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sdev: %" PRIi64 "%s", indent, stat->dev, linesep);
  fprintf (dest, "%sino: %" PRIi64 "%s", indent, stat->ino, linesep);
  fprintf (dest, "%smode: %" PRIi64 "%s", indent, stat->mode, linesep);
  fprintf (dest, "%snlink: %" PRIi64 "%s", indent, stat->nlink, linesep);
  fprintf (dest, "%suid: %" PRIi64 "%s", indent, stat->uid, linesep);
  fprintf (dest, "%sgid: %" PRIi64 "%s", indent, stat->gid, linesep);
  fprintf (dest, "%srdev: %" PRIi64 "%s", indent, stat->rdev, linesep);
  fprintf (dest, "%ssize: %" PRIi64 "%s", indent, stat->size, linesep);
  fprintf (dest, "%sblksize: %" PRIi64 "%s", indent, stat->blksize, linesep);
  fprintf (dest, "%sblocks: %" PRIi64 "%s", indent, stat->blocks, linesep);
  fprintf (dest, "%satime: %" PRIi64 "%s", indent, stat->atime, linesep);
  fprintf (dest, "%smtime: %" PRIi64 "%s", indent, stat->mtime, linesep);
  fprintf (dest, "%sctime: %" PRIi64 "%s", indent, stat->ctime, linesep);
}

void
guestfs_int_print_statns_indent (struct guestfs_statns *statns, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sst_dev: %" PRIi64 "%s", indent, statns->st_dev, linesep);
  fprintf (dest, "%sst_ino: %" PRIi64 "%s", indent, statns->st_ino, linesep);
  fprintf (dest, "%sst_mode: %" PRIi64 "%s", indent, statns->st_mode, linesep);
  fprintf (dest, "%sst_nlink: %" PRIi64 "%s", indent, statns->st_nlink, linesep);
  fprintf (dest, "%sst_uid: %" PRIi64 "%s", indent, statns->st_uid, linesep);
  fprintf (dest, "%sst_gid: %" PRIi64 "%s", indent, statns->st_gid, linesep);
  fprintf (dest, "%sst_rdev: %" PRIi64 "%s", indent, statns->st_rdev, linesep);
  fprintf (dest, "%sst_size: %" PRIi64 "%s", indent, statns->st_size, linesep);
  fprintf (dest, "%sst_blksize: %" PRIi64 "%s", indent, statns->st_blksize, linesep);
  fprintf (dest, "%sst_blocks: %" PRIi64 "%s", indent, statns->st_blocks, linesep);
  fprintf (dest, "%sst_atime_sec: %" PRIi64 "%s", indent, statns->st_atime_sec, linesep);
  fprintf (dest, "%sst_atime_nsec: %" PRIi64 "%s", indent, statns->st_atime_nsec, linesep);
  fprintf (dest, "%sst_mtime_sec: %" PRIi64 "%s", indent, statns->st_mtime_sec, linesep);
  fprintf (dest, "%sst_mtime_nsec: %" PRIi64 "%s", indent, statns->st_mtime_nsec, linesep);
  fprintf (dest, "%sst_ctime_sec: %" PRIi64 "%s", indent, statns->st_ctime_sec, linesep);
  fprintf (dest, "%sst_ctime_nsec: %" PRIi64 "%s", indent, statns->st_ctime_nsec, linesep);
  fprintf (dest, "%sst_spare1: %" PRIi64 "%s", indent, statns->st_spare1, linesep);
  fprintf (dest, "%sst_spare2: %" PRIi64 "%s", indent, statns->st_spare2, linesep);
  fprintf (dest, "%sst_spare3: %" PRIi64 "%s", indent, statns->st_spare3, linesep);
  fprintf (dest, "%sst_spare4: %" PRIi64 "%s", indent, statns->st_spare4, linesep);
  fprintf (dest, "%sst_spare5: %" PRIi64 "%s", indent, statns->st_spare5, linesep);
  fprintf (dest, "%sst_spare6: %" PRIi64 "%s", indent, statns->st_spare6, linesep);
}

void
guestfs_int_print_statvfs_indent (struct guestfs_statvfs *statvfs, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sbsize: %" PRIi64 "%s", indent, statvfs->bsize, linesep);
  fprintf (dest, "%sfrsize: %" PRIi64 "%s", indent, statvfs->frsize, linesep);
  fprintf (dest, "%sblocks: %" PRIi64 "%s", indent, statvfs->blocks, linesep);
  fprintf (dest, "%sbfree: %" PRIi64 "%s", indent, statvfs->bfree, linesep);
  fprintf (dest, "%sbavail: %" PRIi64 "%s", indent, statvfs->bavail, linesep);
  fprintf (dest, "%sfiles: %" PRIi64 "%s", indent, statvfs->files, linesep);
  fprintf (dest, "%sffree: %" PRIi64 "%s", indent, statvfs->ffree, linesep);
  fprintf (dest, "%sfavail: %" PRIi64 "%s", indent, statvfs->favail, linesep);
  fprintf (dest, "%sfsid: %" PRIi64 "%s", indent, statvfs->fsid, linesep);
  fprintf (dest, "%sflag: %" PRIi64 "%s", indent, statvfs->flag, linesep);
  fprintf (dest, "%snamemax: %" PRIi64 "%s", indent, statvfs->namemax, linesep);
}

void
guestfs_int_print_tsk_dirent_indent (struct guestfs_tsk_dirent *tsk_dirent, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%stsk_inode: %" PRIu64 "%s", indent, tsk_dirent->tsk_inode, linesep);
  fprintf (dest, "%stsk_type: %c%s", indent, tsk_dirent->tsk_type, linesep);
  fprintf (dest, "%stsk_size: %" PRIi64 "%s", indent, tsk_dirent->tsk_size, linesep);
  fprintf (dest, "%stsk_name: %s%s", indent, tsk_dirent->tsk_name, linesep);
  fprintf (dest, "%stsk_flags: %" PRIu32 "%s", indent, tsk_dirent->tsk_flags, linesep);
  fprintf (dest, "%stsk_atime_sec: %" PRIi64 "%s", indent, tsk_dirent->tsk_atime_sec, linesep);
  fprintf (dest, "%stsk_atime_nsec: %" PRIi64 "%s", indent, tsk_dirent->tsk_atime_nsec, linesep);
  fprintf (dest, "%stsk_mtime_sec: %" PRIi64 "%s", indent, tsk_dirent->tsk_mtime_sec, linesep);
  fprintf (dest, "%stsk_mtime_nsec: %" PRIi64 "%s", indent, tsk_dirent->tsk_mtime_nsec, linesep);
  fprintf (dest, "%stsk_ctime_sec: %" PRIi64 "%s", indent, tsk_dirent->tsk_ctime_sec, linesep);
  fprintf (dest, "%stsk_ctime_nsec: %" PRIi64 "%s", indent, tsk_dirent->tsk_ctime_nsec, linesep);
  fprintf (dest, "%stsk_crtime_sec: %" PRIi64 "%s", indent, tsk_dirent->tsk_crtime_sec, linesep);
  fprintf (dest, "%stsk_crtime_nsec: %" PRIi64 "%s", indent, tsk_dirent->tsk_crtime_nsec, linesep);
  fprintf (dest, "%stsk_nlink: %" PRIi64 "%s", indent, tsk_dirent->tsk_nlink, linesep);
  fprintf (dest, "%stsk_link: %s%s", indent, tsk_dirent->tsk_link, linesep);
  fprintf (dest, "%stsk_spare1: %" PRIi64 "%s", indent, tsk_dirent->tsk_spare1, linesep);
}

void
guestfs_int_print_utsname_indent (struct guestfs_utsname *utsname, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%suts_sysname: %s%s", indent, utsname->uts_sysname, linesep);
  fprintf (dest, "%suts_release: %s%s", indent, utsname->uts_release, linesep);
  fprintf (dest, "%suts_version: %s%s", indent, utsname->uts_version, linesep);
  fprintf (dest, "%suts_machine: %s%s", indent, utsname->uts_machine, linesep);
}

void
guestfs_int_print_version_indent (struct guestfs_version *version, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%smajor: %" PRIi64 "%s", indent, version->major, linesep);
  fprintf (dest, "%sminor: %" PRIi64 "%s", indent, version->minor, linesep);
  fprintf (dest, "%srelease: %" PRIi64 "%s", indent, version->release, linesep);
  fprintf (dest, "%sextra: %s%s", indent, version->extra, linesep);
}

void
guestfs_int_print_xattr_indent (struct guestfs_xattr *xattr, FILE *dest, const char *linesep, const char *indent)
{
  size_t i;

  fprintf (dest, "%sattrname: %s%s", indent, xattr->attrname, linesep);
  fprintf (dest, "%sattrval: ", indent);
  for (i = 0; i < xattr->attrval_len; ++i)
    if (c_isprint (xattr->attrval[i]))
      fprintf (dest, "%c", xattr->attrval[i]);
    else
      fprintf (dest, "\\x%02x", (unsigned) xattr->attrval[i]);
  fprintf (dest, "%s", linesep);
}

void
guestfs_int_print_xfsinfo_indent (struct guestfs_xfsinfo *xfsinfo, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sxfs_mntpoint: %s%s", indent, xfsinfo->xfs_mntpoint, linesep);
  fprintf (dest, "%sxfs_inodesize: %" PRIu32 "%s", indent, xfsinfo->xfs_inodesize, linesep);
  fprintf (dest, "%sxfs_agcount: %" PRIu32 "%s", indent, xfsinfo->xfs_agcount, linesep);
  fprintf (dest, "%sxfs_agsize: %" PRIu32 "%s", indent, xfsinfo->xfs_agsize, linesep);
  fprintf (dest, "%sxfs_sectsize: %" PRIu32 "%s", indent, xfsinfo->xfs_sectsize, linesep);
  fprintf (dest, "%sxfs_attr: %" PRIu32 "%s", indent, xfsinfo->xfs_attr, linesep);
  fprintf (dest, "%sxfs_blocksize: %" PRIu32 "%s", indent, xfsinfo->xfs_blocksize, linesep);
  fprintf (dest, "%sxfs_datablocks: %" PRIu64 "%s", indent, xfsinfo->xfs_datablocks, linesep);
  fprintf (dest, "%sxfs_imaxpct: %" PRIu32 "%s", indent, xfsinfo->xfs_imaxpct, linesep);
  fprintf (dest, "%sxfs_sunit: %" PRIu32 "%s", indent, xfsinfo->xfs_sunit, linesep);
  fprintf (dest, "%sxfs_swidth: %" PRIu32 "%s", indent, xfsinfo->xfs_swidth, linesep);
  fprintf (dest, "%sxfs_dirversion: %" PRIu32 "%s", indent, xfsinfo->xfs_dirversion, linesep);
  fprintf (dest, "%sxfs_dirblocksize: %" PRIu32 "%s", indent, xfsinfo->xfs_dirblocksize, linesep);
  fprintf (dest, "%sxfs_cimode: %" PRIu32 "%s", indent, xfsinfo->xfs_cimode, linesep);
  fprintf (dest, "%sxfs_logname: %s%s", indent, xfsinfo->xfs_logname, linesep);
  fprintf (dest, "%sxfs_logblocksize: %" PRIu32 "%s", indent, xfsinfo->xfs_logblocksize, linesep);
  fprintf (dest, "%sxfs_logblocks: %" PRIu32 "%s", indent, xfsinfo->xfs_logblocks, linesep);
  fprintf (dest, "%sxfs_logversion: %" PRIu32 "%s", indent, xfsinfo->xfs_logversion, linesep);
  fprintf (dest, "%sxfs_logsectsize: %" PRIu32 "%s", indent, xfsinfo->xfs_logsectsize, linesep);
  fprintf (dest, "%sxfs_logsunit: %" PRIu32 "%s", indent, xfsinfo->xfs_logsunit, linesep);
  fprintf (dest, "%sxfs_lazycount: %" PRIu32 "%s", indent, xfsinfo->xfs_lazycount, linesep);
  fprintf (dest, "%sxfs_rtname: %s%s", indent, xfsinfo->xfs_rtname, linesep);
  fprintf (dest, "%sxfs_rtextsize: %" PRIu32 "%s", indent, xfsinfo->xfs_rtextsize, linesep);
  fprintf (dest, "%sxfs_rtblocks: %" PRIu64 "%s", indent, xfsinfo->xfs_rtblocks, linesep);
  fprintf (dest, "%sxfs_rtextents: %" PRIu64 "%s", indent, xfsinfo->xfs_rtextents, linesep);
}

void
guestfs_int_print_yara_detection_indent (struct guestfs_yara_detection *yara_detection, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%syara_name: %s%s", indent, yara_detection->yara_name, linesep);
  fprintf (dest, "%syara_rule: %s%s", indent, yara_detection->yara_rule, linesep);
}

#if GUESTFS_PRIVATE

void
guestfs_int_print_internal_mountable_indent (struct guestfs_internal_mountable *internal_mountable, FILE *dest, const char *linesep, const char *indent)
{
  fprintf (dest, "%sim_type: %" PRIi32 "%s", indent, internal_mountable->im_type, linesep);
  fprintf (dest, "%sim_device: %s%s", indent, internal_mountable->im_device, linesep);
  fprintf (dest, "%sim_volume: %s%s", indent, internal_mountable->im_volume, linesep);
}

#endif /* End of GUESTFS_PRIVATE. */
