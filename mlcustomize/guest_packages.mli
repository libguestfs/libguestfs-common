(* virt-customize
 * Copyright (C) 2012-2021 Red Hat Inc.
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

exception Unknown_package_manager of string
exception Unimplemented_package_manager of string
(** For all three functions below, [package_management] determines the package
    management system in use by the guest; commonly it should be filled in from
    [Guestfs.inspect_get_package_management], or the equivalent guestfs object
    method.

    If [package_management] is unknown or unimplemented, the functions raise
    [Unknown_package_manager "error message"] or [Unimplemented_package_manager
    "error message"], correspondingly. *)

val install_command : string list -> string -> string
(** [install_command packages package_management] produces a properly quoted
    shell command string suitable for execution in the guest (directly or via a
    Firstboot script) for installing the OS packages listed in [packages]. *)

val update_command : string -> string
(** [update_command package_management] produces a properly quoted shell command
    string suitable for execution in the guest (directly or via a Firstboot
    script) for updating the OS packages that are currently installed in the
    guest. *)

val uninstall_command : string list -> string -> string
(** [uninstall_command packages package_management] produces a properly quoted
    shell command string suitable for execution in the guest (directly or via a
    Firstboot script) for uninstalling the OS packages listed in [packages]. *)
