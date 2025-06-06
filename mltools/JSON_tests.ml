(* mltools JSON tests
 * Copyright (C) 2015-2025 Red Hat Inc.
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

(* This file tests the JSON module. *)

open Std_utils

let assert_equal ~printer a b =
  if a <> b then
    failwithf "FAIL: %s <> %s" (printer a) (printer b)
let assert_equal_string = assert_equal ~printer:Fun.id

(* "basic" suite. *)
let () =
  let doc = [] in
  assert_equal_string "{}" (JSON.string_of_doc doc);
  assert_equal_string "{
}" (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* string *)
let () =
  let doc = [ "test_string", JSON.String "foo"; ] in
  assert_equal_string "{ \"test_string\": \"foo\" }"
    (JSON.string_of_doc doc);
  assert_equal_string "{
  \"test_string\": \"foo\"
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* bool *)
let () =
  let doc = [ "test_true", JSON.Bool true;
              "test_false", JSON.Bool false ] in
  assert_equal_string
    "{ \"test_true\": true, \"test_false\": false }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"test_true\": true,
  \"test_false\": false
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* int *)
let () =
  let doc = [ "test_zero", JSON.Int 0L;
              "test_pos", JSON.Int 5L;
              "test_neg", JSON.Int (-5L);
              "test_pos64", JSON.Int 1_000_000_000_000L;
              "test_neg64", JSON.Int (-1_000_000_000_000L); ] in
  assert_equal_string
    "{ \"test_zero\": 0, \"test_pos\": 5, \"test_neg\": -5, \"test_pos64\": 1000000000000, \"test_neg64\": -1000000000000 }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"test_zero\": 0,
  \"test_pos\": 5,
  \"test_neg\": -5,
  \"test_pos64\": 1000000000000,
  \"test_neg64\": -1000000000000
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* float *)
let () =
  let doc = [ "test_zero", JSON.Float 0.;
              "test_one", JSON.Float 1.;
              "test_frac", JSON.Float 1.5;
              "test_neg_frac", JSON.Float (-1.5);
              "test_exp", JSON.Float 1e100 ] in
  assert_equal_string
    "{ \"test_zero\": 0, \"test_one\": 1, \"test_frac\": 1.5, \"test_neg_frac\": -1.5, \"test_exp\": 1e+100 }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"test_zero\": 0,
  \"test_one\": 1,
  \"test_frac\": 1.5,
  \"test_neg_frac\": -1.5,
  \"test_exp\": 1e+100
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* list *)
let () =
  let doc = [ "item", JSON.List [ JSON.String "foo"; JSON.Int 10L; JSON.Bool true ] ] in
  assert_equal_string
    "{ \"item\": [ \"foo\", 10, true ] }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"item\": [
    \"foo\",
    10,
    true
  ]
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* nested dict *)
let () =
  let doc = [
      "item", JSON.Dict [ "int", JSON.Int 5L; "string", JSON.String "foo"; ];
      "last", JSON.Int 10L;
    ] in
  assert_equal_string
    "{ \"item\": { \"int\": 5, \"string\": \"foo\" }, \"last\": 10 }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"item\": {
    \"int\": 5,
    \"string\": \"foo\"
  },
  \"last\": 10
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* nested x2 dict *)
let () =
  let doc = [
      "item", JSON.Dict [ "int", JSON.Int 5L;
        "item2", JSON.Dict [ "int", JSON.Int 0L; ];
      ];
      "last", JSON.Int 10L;
    ] in
  assert_equal_string
    "{ \"item\": { \"int\": 5, \"item2\": { \"int\": 0 } }, \"last\": 10 }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"item\": {
    \"int\": 5,
    \"item2\": {
      \"int\": 0
    }
  },
  \"last\": 10
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* escapes *)
let () =
  let doc = [ "test_string", JSON.String "test \" ' \n \b \r \t"; ] in
  assert_equal_string "{ \"test_string\": \"test \\\" ' \\n \\b \\r \\t\" }"
    (JSON.string_of_doc doc);
  assert_equal_string "{
  \"test_string\": \"test \\\" ' \\n \\b \\r \\t\"
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* "examples" suite. *)
let () =
  let doc = [
    "file.driver", JSON.String "https";
    "file.url", JSON.String "https://libguestfs.org";
    "file.timeout", JSON.Int 60L;
    "file.readahead", JSON.Int (64L *^ 1024L *^ 1024L);
  ] in
  assert_equal_string
    "{ \"file.driver\": \"https\", \"file.url\": \"https://libguestfs.org\", \"file.timeout\": 60, \"file.readahead\": 67108864 }"
    (JSON.string_of_doc doc);
  assert_equal_string
    "{
  \"file.driver\": \"https\",
  \"file.url\": \"https://libguestfs.org\",
  \"file.timeout\": 60,
  \"file.readahead\": 67108864
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)

(* builder *)
let () =
  let doc = [
    "version", JSON.Int 1L;
    "sources", JSON.List [
      JSON.Dict [
        "uri", JSON.String "http://libguestfs.org/index";
      ];
    ];
    "templates", JSON.List [
      JSON.Dict [
        "os-version", JSON.String "phony-debian";
        "full-name", JSON.String "Phony Debian";
        "arch", JSON.String "x86_64";
        "size", JSON.Int 536870912_L;
        "notes", JSON.Dict [
          "C", JSON.String "Phony Debian look-alike used for testing.";
        ];
        "hidden", JSON.Bool false;
      ];
      JSON.Dict [
        "os-version", JSON.String "phony-fedora";
        "full-name", JSON.String "Phony Fedora";
        "arch", JSON.String "x86_64";
        "size", JSON.Int 1073741824_L;
        "notes", JSON.Dict [
          "C", JSON.String "Phony Fedora look-alike used for testing.";
        ];
        "hidden", JSON.Bool false;
      ];
    ];
  ] in
  assert_equal_string
    "{
  \"version\": 1,
  \"sources\": [
    {
      \"uri\": \"http://libguestfs.org/index\"
    }
  ],
  \"templates\": [
    {
      \"os-version\": \"phony-debian\",
      \"full-name\": \"Phony Debian\",
      \"arch\": \"x86_64\",
      \"size\": 536870912,
      \"notes\": {
        \"C\": \"Phony Debian look-alike used for testing.\"
      },
      \"hidden\": false
    },
    {
      \"os-version\": \"phony-fedora\",
      \"full-name\": \"Phony Fedora\",
      \"arch\": \"x86_64\",
      \"size\": 1073741824,
      \"notes\": {
        \"C\": \"Phony Fedora look-alike used for testing.\"
      },
      \"hidden\": false
    }
  ]
}"
    (JSON.string_of_doc ~fmt:JSON.Indented doc)
