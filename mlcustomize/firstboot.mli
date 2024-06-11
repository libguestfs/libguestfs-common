(* virt-customize
 * Copyright (C) 2012-2019 Red Hat Inc.
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

val add_firstboot_script : Guestfs.guestfs -> string -> ?prio:int -> string ->
                           string -> unit
  (** [add_firstboot_script g root prio name content] adds a firstboot
      script called [name] containing [content] with priority [prio].

      [g] is the guestfs handle.  The disks must be mounted up and
      inspection data must be available.

      [content] is the contents of the script, {b not} a filename.

      The script is running using the shell (usually [/bin/sh]) on Linux
      or as a Windows batch file.  To use Windows Powershell, see
      {!add_firstboot_powershell} instead.

      The actual name of the script on the guest filesystem is made of [name]
      with all characters but alphanumeric replaced with dashes.

      Within a given priority, the scripts are run in the order they are
      registered. A group of scripts with a numerically lower priority is run
      before a group with a numerically greater priority. If [prio] is omitted,
      it is taken as 5000. If [prio] is smaller than 0 or greater than 9999, an
      Assert_failure is raised (the [prio] parameter is not expected to depend
      on user input).

      For Linux guests using SELinux you should make sure the
      filesystem is relabelled after calling this. *)

val add_firstboot_powershell : Guestfs.guestfs -> string ->
                               ?prio:int -> string -> string list -> unit
(** [add_firstboot_powershell] is like {!add_firstboot_script} except
    that it adds a Windows Powershell script instead of a batch
    file.

    The parameters are: [g root prio name lines] (where the Powershell
    script is passed in as lines of code). *)
