(* Augeas OCaml bindings
 * Copyright (C) 2008 Red Hat Inc., Richard W.M. Jones
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
 *
 * $Id: test_augeas.ml,v 1.1 2008/05/06 10:48:20 rjones Exp $
 *)

let () =
  let aug =
    let loadpath = None in
    let flags = [ Augeas.AugSaveBackup ] in
    Augeas.create "test_root" loadpath flags in

  (* Print all files recursively. *)
  let rec print path =
    if path <> "" then (
      let value = Augeas.get aug path in
      match value with
      | None -> print_endline path
      | Some value -> Printf.printf "%s -> '%s'\n%!" path value
    );
    let files = List.sort compare (Augeas.matches aug (path ^ "/*")) in
    List.iter print files
  in
  print "";

  (* Run the garbage collector to check for internal errors. *)
  Gc.compact ()
