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

open Printf

open Common_gettext.Gettext
open Std_utils

exception Unknown_package_manager of string
exception Unimplemented_package_manager of string

(* Windows has package_management == "unknown". *)
let error_unknown_package_manager flag =
  let msg = sprintf (f_"cannot use ‘%s’ because no package manager has been \
                        detected for this guest OS.\n\nIf this guest OS is a \
                        common one with ordinary package management then this \
                        may have been caused by a failure of libguestfs \
                        inspection.\n\nFor OSes such as Windows that lack \
                        package management, this is not possible.  Try using \
                        one of the ‘--firstboot*’ flags instead (described in \
                        the virt-customize(1) manual).") flag in
  raise (Unknown_package_manager msg)

let error_unimplemented_package_manager flag pm =
  let msg = sprintf (f_"sorry, ‘%s’ with the ‘%s’ package manager has not \
                        been implemented yet.\n\nYou can work around this by \
                        using one of the ‘--run*’ or ‘--firstboot*’ options \
                        instead (described in the virt-customize(1) manual).")
                    flag pm in
  raise (Unimplemented_package_manager msg)

(* http://distrowatch.com/dwres.php?resource=package-management *)
let install_command packages package_management =
  let quoted_args = String.concat " " (List.map quote packages) in
  match package_management with
  | "apk" ->
     sprintf "
       apk update
       apk add %s
     " quoted_args
  | "apt" ->
    (* http://unix.stackexchange.com/questions/22820 *)
    sprintf "
      export DEBIAN_FRONTEND=noninteractive
      apt_opts='-q -y -o Dpkg::Options::=--force-confnew'
      apt-get $apt_opts update
      apt-get $apt_opts install %s
    " quoted_args
  | "dnf" ->
     sprintf "dnf%s -y install %s"
             (if verbose () then " --verbose" else "")
             quoted_args
  | "pisi" ->   sprintf "pisi it %s" quoted_args
  | "pacman" -> sprintf "pacman -S --noconfirm %s" quoted_args
  | "urpmi" ->  sprintf "urpmi %s" quoted_args
  | "xbps" ->   sprintf "xbps-install -Sy %s" quoted_args
  | "yum" ->    sprintf "yum -y install %s" quoted_args
  | "zypper" -> sprintf "zypper -n in -l %s" quoted_args

  | "unknown" ->
    error_unknown_package_manager (s_"--install")
  | pm ->
    error_unimplemented_package_manager (s_"--install") pm

let update_command package_management =
  match package_management with
  | "apk" ->
     "
       apk update
       apk upgrade
     "
  | "apt" ->
    (* http://unix.stackexchange.com/questions/22820 *)
    "
      export DEBIAN_FRONTEND=noninteractive
      apt_opts='-q -y -o Dpkg::Options::=--force-confnew'
      apt-get $apt_opts update
      apt-get $apt_opts upgrade
    "
  | "dnf" ->
     sprintf "dnf%s -y --best upgrade"
             (if verbose () then " --verbose" else "")
  | "pisi" ->   "pisi upgrade"
  | "pacman" -> "pacman -Su"
  | "urpmi" ->  "urpmi --auto-select"
  | "xbps" ->   "xbps-install -Suy"
  | "yum" ->    "yum -y update"
  | "zypper" -> "zypper -n update -l"

  | "unknown" ->
    error_unknown_package_manager (s_"--update")
  | pm ->
    error_unimplemented_package_manager (s_"--update") pm

let uninstall_command packages package_management =
  let quoted_args = String.concat " " (List.map quote packages) in
  match package_management with
  | "apk" -> sprintf "apk del %s" quoted_args
  | "apt" ->
    (* http://unix.stackexchange.com/questions/22820 *)
    sprintf "
      export DEBIAN_FRONTEND=noninteractive
      apt_opts='-q -y -o Dpkg::Options::=--force-confnew'
      apt-get $apt_opts remove %s
    " quoted_args
  | "dnf" ->    sprintf "dnf -y remove %s" quoted_args
  | "pisi" ->   sprintf "pisi rm %s" quoted_args
  | "pacman" -> sprintf "pacman -R %s" quoted_args
  | "urpmi" ->  sprintf "urpme %s" quoted_args
  | "xbps" ->   sprintf "xbps-remove -Sy %s" quoted_args
  | "yum" ->    sprintf "yum -y remove %s" quoted_args
  | "zypper" -> sprintf "zypper -n rm %s" quoted_args

  | "unknown" ->
    error_unknown_package_manager (s_"--uninstall")
  | pm ->
    error_unimplemented_package_manager (s_"--uninstall") pm
