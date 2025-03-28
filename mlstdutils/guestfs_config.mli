(* mlstdutils
 * Copyright (C) 2017 Red Hat Inc.
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

val package_name : string
(** The configure value [@PACKAGE_NAME@] *)

val package_version : string
(** The configure value [@PACKAGE_VERSION@] *)

val package_version_full : string
(** The configure value [@PACKAGE_VERSION_FULL@] *)

val prefix : string
(** The configure value [@prefix@] *)

val datadir : string
(** The configure value [@datadir@] *)

val host_os : string
(** The configure value [@host_os@] *)

val host_cpu : string
(** The configure value [@host_cpu@] *)
