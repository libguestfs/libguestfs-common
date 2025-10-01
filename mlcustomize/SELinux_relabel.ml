(* virt-customize
 * Copyright (C) 2016-2025 Red Hat Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *)

open Std_utils
open Tools_utils
open Common_gettext.Gettext

open Printf

module G = Guestfs

(* XXX A lot of this code could usefully be moved into
 * [libguestfs.git/daemon/selinux.ml].
 *)

let rec relabel (g : G.guestfs) =
  (* Is the guest using SELinux?  (Otherwise this is a no-op). *)
  if is_selinux_guest g then (
    try
      use_setfiles g;
      (* That worked, so we don't need to autorelabel. *)
      g#rm_f "/.autorelabel"
    with Failure _ ->
      (* This is the fallback in case something in the setfiles
       * method didn't work.  That includes the case where a non-SELinux
       * host is processing an SELinux guest, and other things.
       *)
      g#touch "/.autorelabel"
  )

and is_selinux_guest g =
  g#is_file ~followsymlinks:true "/usr/sbin/load_policy" &&
  g#is_file ~followsymlinks:true "/etc/selinux/config"

and use_setfiles g =
  (* Is setfiles / SELinux relabelling functionality available? *)
  if not (g#feature_available [| "selinuxrelabel" |]) then
    failwith "no selinux relabel feature";

  (* Use Augeas to parse /etc/selinux/config. *)
  g#aug_init "/" (16+32) (* AUG_SAVE_NOOP | AUG_NO_LOAD *);
  (* See: https://bugzilla.redhat.com/show_bug.cgi?id=975412#c0 *)
  ignore (g#aug_rm "/augeas/load/*[\"/etc/selinux/config/\" !~ regexp('^') + glob(incl) + regexp('/.*')]");
  g#aug_load ();
  debug_augeas_errors g;

  let config_path = "/files/etc/selinux/config" in
  let config_keys = g#aug_ls config_path in
  (* SELinux may be disabled via a setting in config file *)
  let selinux_disabled =
    let selinuxmode_path = config_path ^ "/SELINUX" in
    if Array.mem selinuxmode_path config_keys then
      g#aug_get selinuxmode_path = "disabled"
    else
      false in
  if selinux_disabled then
      failwith "selinux disabled";

  (* Get the SELinux policy name, eg. "targeted", "minimum".
   * Use "targeted" if not specified, just like libselinux does.
   *)
  let policy =
    let selinuxtype_path = config_path ^ "/SELINUXTYPE" in
    if Array.mem selinuxtype_path config_keys then
      g#aug_get selinuxtype_path
    else
      "targeted" in

  g#aug_close ();

  (* Get the spec file name. *)
  let specfile =
    sprintf "/etc/selinux/%s/contexts/files/file_contexts" policy in

  (* If the spec file doesn't exist then fall back to using
   * autorelabel (RHBZ#1828952).
   *)
  if not (g#is_file ~followsymlinks:true specfile) then
    failwith "no spec file";

  (* RHEL 6.2 - 6.5 had a malformed specfile that contained the
   * invalid regular expression "/var/run/spice-vdagentd.\pid"
   * (instead of "\.p").  This stops setfiles from working on
   * the guest.
   *
   * Because an SELinux relabel writes all over the filesystem,
   * it seems reasonable to fix this problem in the specfile
   * at the same time.  (RHBZ#1374232)
   *)
  if g#grep ~fixed:true "vdagentd.\\pid" specfile <> [||] then (
    debug "fixing invalid regular expression in %s" specfile;
    let old_specfile = specfile ^ "~" in
    g#mv specfile old_specfile;
    let content = g#read_file old_specfile in
    let content =
      String.replace content "vdagentd.\\pid" "vdagentd\\.pid" in
    g#write specfile content;
    g#copy_attributes ~all:true old_specfile specfile
  );

  (* Relabel everything. *)
  g#setfiles ~force:true specfile ["/"]
