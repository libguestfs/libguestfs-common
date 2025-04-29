(* Utilities for OCaml tools in libguestfs.
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

(* This file tests the Std_utils module. *)

open Printf

open Std_utils

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

let assert_equal_string =
  assert_equal ~printer:identity
let assert_equal_int =
  assert_equal ~printer:(fun x -> string_of_int x)
let assert_equal_int64 =
  assert_equal ~printer:(fun x -> Int64.to_string x)
let assert_equal_stringlist =
  assert_equal ~printer:(fun x -> "(" ^ (String.escaped (String.concat "," x)) ^ ")")
let assert_equal_stringpair =
  assert_equal ~printer:(fun (x, y) -> sprintf "%S, %S" x y)
let assert_nonempty_string str =
  if str = "" then (
    eprintf "Expected empty string, got '%s'\n" str;
    exit 1
  )
let assert_raises_executable_not_found exe =
  assert_raises (Executable_not_found exe) (fun () -> which exe)

let assert_bool name b =
  if not b then failwithf "FAIL: %s" name

(* Test Std_utils.int_of_X and Std_utils.X_of_int byte swapping
 * functions.
 *)
let rec test_byteswap () =
  test_swap int_of_le16 le16_of_int 0x2040L "\x40\x20";
  test_swap int_of_le32 le32_of_int 0x20406080L "\x80\x60\x40\x20";
  test_swap int_of_le64 le64_of_int
            0x20406080A0C0E0F0L "\xF0\xE0\xC0\xA0\x80\x60\x40\x20";
  test_swap int_of_be16 be16_of_int 0x2040L "\x20\x40";
  test_swap int_of_be32 be32_of_int 0x20406080L "\x20\x40\x60\x80";
  test_swap int_of_be64 be64_of_int
            0x20406080A0C0E0F0L "\x20\x40\x60\x80\xA0\xC0\xE0\xF0"

and test_swap int_of_x x_of_int i s =
  assert_equal_int64 i (int_of_x s);
  assert_equal_string s (x_of_int i)

let () = test_byteswap ()

(* Test Std_utils.Char.mem. *)
let () =
  assert_bool "Char.mem" (Char.mem 'a' "abc");
  assert_bool "Char.mem" (Char.mem 'b' "abc");
  assert_bool "Char.mem" (Char.mem 'c' "abc");
  assert_bool "Char.mem" (not (Char.mem 'd' "abc"));
  assert_bool "Char.mem" (not (Char.mem 'a' ""))

(* Test Std_utils.String.is_prefix. *)
let () =
  assert_bool "String.is_prefix,," (String.is_prefix "" "");
  assert_bool "String.is_prefix,foo," (String.is_prefix "foo" "");
  assert_bool "String.is_prefix,foo,foo" (String.is_prefix "foo" "foo");
  assert_bool "String.is_prefix,foo123,foo" (String.is_prefix "foo123" "foo");
  assert_bool "not (String.is_prefix,,foo" (not (String.is_prefix "" "foo"))

(* Test Std_utils.String.is_suffix. *)
let () =
  assert_bool "String.is_suffix,," (String.is_suffix "" "");
  assert_bool "String.is_suffix,foo," (String.is_suffix "foo" "");
  assert_bool "String.is_suffix,foo,foo" (String.is_suffix "foo" "foo");
  assert_bool "String.is_suffix,123foo,foo" (String.is_suffix "123foo" "foo");
  assert_bool "not String.is_suffix,,foo" (not (String.is_suffix "" "foo"))

(* Test Std_utils.String.find. *)
let () =
  assert_equal_int 0 (String.find "" "");
  assert_equal_int 0 (String.find "foo" "");
  assert_equal_int 1 (String.find "foo" "o");
  assert_equal_int 3 (String.find "foobar" "bar");
  assert_equal_int (-1) (String.find "" "baz");
  assert_equal_int (-1) (String.find "foobar" "baz")

(* Test Std_utils.String.break. *)
let () =
  assert_equal_stringpair ("a", "b") (String.break 1 "ab");
  assert_equal_stringpair ("", "ab") (String.break 0 "ab");
  assert_equal_stringpair ("", "ab") (String.break (-1) "ab");
  assert_equal_stringpair ("ab", "") (String.break 2 "ab");
  assert_equal_stringpair ("ab", "") (String.break 3 "ab");
  assert_equal_stringpair ("abc", "def") (String.break 3 "abcdef");
  assert_equal_stringpair ("", "") (String.break 0 "");
  assert_equal_stringpair ("", "") (String.break (-2) "");
  assert_equal_stringpair ("", "") (String.break 2 "")

(* Test Std_utils.String.split. *)
let () =
  assert_equal_stringpair ("a", "b") (String.split " " "a b");
  assert_equal_stringpair ("", "ab") (String.split " " " ab");
  assert_equal_stringpair ("", "abc") (String.split "" "abc");
  assert_equal_stringpair ("abc", "") (String.split " " "abc");
  assert_equal_stringpair ("", "") (String.split " " "")

(* Test Std_utils.String.nsplit. *)
let () =
  (* XXX Not clear if the next test case indicates an error in
   * String.nsplit.  However this is how it has historically worked.
   *)
  assert_equal_stringlist [""] (String.nsplit " " "");
  assert_equal_stringlist ["abc"] (String.nsplit " " "abc");
  assert_equal_stringlist ["a"; "b"; "c"] (String.nsplit " " "a b c");
  assert_equal_stringlist ["abc"; "d"; "e"] (String.nsplit " " "abc d e");
  assert_equal_stringlist ["a"; "b"; "c"; ""] (String.nsplit " " "a b c ");
  assert_equal_stringlist [""; "a"; "b"; "c"] (String.nsplit " " " a b c");
  assert_equal_stringlist [""; "a"; "b"; "c"; ""] (String.nsplit " " " a b c ");
  assert_equal_stringlist ["a b c d"] (String.nsplit ~max:1 " " "a b c d");
  assert_equal_stringlist ["a"; "b c d"] (String.nsplit ~max:2 " " "a b c d");
  assert_equal_stringlist ["a"; "b"; "c d"] (String.nsplit ~max:3 " " "a b c d");
  assert_equal_stringlist ["a"; "b"; "c"; "d"] (String.nsplit ~max:10 " " "a b c d");

  (* Test that nsplit can handle large strings. *)
  let xs = Array.to_list (Array.make 10_000_000 "xyz") in
  let xs_concat = String.concat " " xs in
  assert_equal_stringlist xs (String.nsplit " " xs_concat)

(* Test Std_utils.String.lines_split. *)
let () =
  assert_equal_stringlist [""] (String.lines_split "");
  assert_equal_stringlist ["A"] (String.lines_split "A");
  assert_equal_stringlist ["A"; ""] (String.lines_split "A\n");
  assert_equal_stringlist ["A"; "B"] (String.lines_split "A\nB");
  assert_equal_stringlist ["A"; "B"; "C"] (String.lines_split "A\nB\nC");
  assert_equal_stringlist ["A"; "B"; "C"; "D"] (String.lines_split "A\nB\nC\nD");
  assert_equal_stringlist ["A\n"] (String.lines_split "A\\");
  assert_equal_stringlist ["A\nB"] (String.lines_split "A\\\nB");
  assert_equal_stringlist ["A"; "B\nC"] (String.lines_split "A\nB\\\nC");
  assert_equal_stringlist ["A"; "B\nC"; "D"] (String.lines_split "A\nB\\\nC\nD");
  assert_equal_stringlist ["A"; "B\nC\nD"] (String.lines_split "A\nB\\\nC\\\nD");
  assert_equal_stringlist ["A\nB"; ""] (String.lines_split "A\\\nB\n");
  assert_equal_stringlist ["A\nB\n"] (String.lines_split "A\\\nB\\\n")

(* Test Std_utils.String.span and cspan. *)
let () =
  assert_equal_int 3 (String.span "aaabb" "a");
  assert_equal_int 3 (String.span "aaaba" "a");
  assert_equal_int 3 (String.span "aba" "ab");
  assert_equal_int 0 (String.span "" "ab");
  assert_equal_int 3 (String.cspan "defab" "ab");
  assert_equal_int 3 (String.cspan "defba" "ab");
  assert_equal_int 3 (String.cspan "def" "ab");
  assert_equal_int 0 (String.cspan "" "ab")

(* Test Std_utils.String.chomp. *)
let () =
  assert_equal_string "a" (String.chomp "a");
  assert_equal_string "a" (String.chomp "a\n");
  assert_equal_string "a\nb" (String.chomp "a\nb");
  assert_equal_string "" (String.chomp "");
  assert_equal_string "" (String.chomp "\n");
  assert_equal_string "\n" (String.chomp "\n\n") (* only removes one *)

(* Test Std_utils.which. *)
let () =
  assert_nonempty_string (which "true");
  assert_raises_executable_not_found "this-command-does-not-really-exist";
  begin
    let exe_name = "true" in
    let exe = which exe_name in
    assert_equal_string exe (which exe);
    let pwd = Sys.getcwd () in
    Sys.chdir (Filename.dirname exe);
    let exe_relative = "./" ^ exe_name in
    assert_equal_string exe_relative (which exe_relative);
    Sys.chdir pwd
  end;
  ()

(* Test List.make. *)
let () =
  assert_equal_stringlist [] (List.make 0 "1");
  assert_equal_stringlist ["1"; "1"; "1"] (List.make 3 "1");
  assert_raises (Invalid_argument "make") (fun () -> List.make (-1) "1")
