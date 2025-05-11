(* virt-v2v
 * Copyright (C) 2011-2025 Red Hat Inc.
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

(* This file tests individual OCaml bindings for C utility functions. *)

open Printf

open Std_utils
open C_utils

let assert_equal ~printer a b =
  if a <> b then
    failwithf "FAIL: %s <> %s" (printer a) (printer b)

let assert_raises exn fn =
  try
    fn ();
    failwithf "FAIL: expected function to raise an exception"
  with exn' ->
    if exn <> exn' then (
      eprintf "FAIL: function raised the wrong exception:\n\
               expected %s\n\
               actual %s\n"
        (Printexc.to_string exn) (Printexc.to_string exn');
      exit 1
    )

(* Test drive_name function. *)
let () =
  let printer = Fun.id in
  assert_equal ~printer "a" (drive_name 0);
  assert_equal ~printer "z" (drive_name 25);
  assert_equal ~printer "aa" (drive_name 26);
  assert_equal ~printer "ab" (drive_name 27);
  assert_equal ~printer "az" (drive_name 51);
  assert_equal ~printer "ba" (drive_name 52);
  assert_equal ~printer "zz" (drive_name 701);
  assert_equal ~printer "aaa" (drive_name 702);
  assert_equal ~printer "zzz" (drive_name 18277)

(* Test drive_index function. *)
let () =
  let printer = string_of_int in
  assert_equal ~printer 0 (drive_index "a");
  assert_equal ~printer 25 (drive_index "z");
  assert_equal ~printer 26 (drive_index "aa");
  assert_equal ~printer 27 (drive_index "ab");
  assert_equal ~printer 51 (drive_index "az");
  assert_equal ~printer 52 (drive_index "ba");
  assert_equal ~printer 701 (drive_index "zz");
  assert_equal ~printer 702 (drive_index "aaa");
  assert_equal ~printer 18277 (drive_index "zzz");
  let exn = Invalid_argument "drive_index: invalid parameter" in
  assert_raises exn (fun () -> drive_index "");
  assert_raises exn (fun () -> drive_index "abc123");
  assert_raises exn (fun () -> drive_index "123");
  assert_raises exn (fun () -> drive_index "Z");
  assert_raises exn (fun () -> drive_index "aB")

(* Test shell_unquote function. *)
let () =
  let printer = Fun.id in
  assert_equal ~printer "a" (shell_unquote "a");
  assert_equal ~printer "b" (shell_unquote "'b'");
  assert_equal ~printer "c" (shell_unquote "\"c\"");
  assert_equal ~printer "dd" (shell_unquote "\"dd\"");
  assert_equal ~printer "e\\e" (shell_unquote "\"e\\\\e\"");
  assert_equal ~printer "f\\" (shell_unquote "\"f\\\\\"");
  assert_equal ~printer "\\g" (shell_unquote "\"\\\\g\"");
  assert_equal ~printer "h\\-h" (shell_unquote "\"h\\-h\"");
  assert_equal ~printer "i`" (shell_unquote "\"i\\`\"");
  assert_equal ~printer "j\"" (shell_unquote "\"j\\\"\"")
