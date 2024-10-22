/* JSON parser
 * Copyright (C) 2015-2019 Red Hat Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <config.h>

#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>

#include <json.h>

#include <stdio.h>
#include <string.h>

#define JSON_NULL       (Val_int (0)) /* Variants without parameters. */
#define JSON_STRING_TAG 0             /* Variants with parameters. */
#define JSON_INT_TAG    1
#define JSON_FLOAT_TAG  2
#define JSON_BOOL_TAG   3
#define JSON_LIST_TAG   4
#define JSON_DICT_TAG   5

value virt_builder_json_parser_tree_parse (value stringv);
value virt_builder_json_parser_tree_parse_file (value stringv);

static value
convert_json_t (json_object *val, int level)
{
  CAMLparam0 ();
  CAMLlocal5 (rv, v, tv, sv, consv);

  if (level > 20)
    caml_invalid_argument ("too many levels of object/array nesting");

  switch (json_object_get_type (val)) {
  case json_type_object: {
    struct json_object_iterator it, itend;
    const char *key;
    json_object *jvalue;

    rv = caml_alloc (1, JSON_DICT_TAG);
    v = Val_int (0);
    /* This will create the OCaml list backwards, but JSON
     * dictionaries are supposed to be unordered so that shouldn't
     * matter, right?  Well except that for some consumers this does
     * matter (eg. simplestreams which incorrectly uses a dict when it
     * really should use an array).
     */
    it = json_object_iter_begin (val);
    itend = json_object_iter_end (val);
    while (!json_object_iter_equal (&it, &itend)) {
      key = json_object_iter_peek_name (&it);
      tv = caml_alloc_tuple (2);
      sv = caml_copy_string (key);
      Store_field (tv, 0, sv);

      jvalue = json_object_iter_peek_value (&it);
      sv = convert_json_t (jvalue, level + 1);
      Store_field (tv, 1, sv);

      consv = caml_alloc (2, 0);
      Store_field (consv, 1, v);
      Store_field (consv, 0, tv);
      v = consv;

      json_object_iter_next (&it);
    }
    Store_field (rv, 0, v);
    break;
  }

  case json_type_array: {
    const size_t len = json_object_array_length (val);
    size_t i;
    json_object *jvalue;

    rv = caml_alloc (1, JSON_LIST_TAG);
    v = Val_int (0);
    for (i = 0; i < len; ++i) {
      /* Note we have to create the OCaml list backwards. */
      jvalue = json_object_array_get_idx (val, len-i-1);
      tv = convert_json_t (jvalue, level + 1);
      consv = caml_alloc (2, 0);
      Store_field (consv, 1, v);
      Store_field (consv, 0, tv);
      v = consv;
    }
    Store_field (rv, 0, v);
    break;
  }

  case json_type_string:
    rv = caml_alloc (1, JSON_STRING_TAG);
    v = caml_copy_string (json_object_get_string (val));
    Store_field (rv, 0, v);
    break;

  case json_type_double:
    rv = caml_alloc (1, JSON_FLOAT_TAG);
    v = caml_copy_double (json_object_get_double (val));
    Store_field (rv, 0, v);
    break;

  case json_type_int:
    rv = caml_alloc (1, JSON_INT_TAG);
    v = caml_copy_int64 (json_object_get_int64 (val));
    Store_field (rv, 0, v);
    break;

  case json_type_boolean:
    rv = caml_alloc (1, JSON_BOOL_TAG);
    Store_field (rv, 0, json_object_get_boolean (val) ? Val_true : Val_false);
    break;

  case json_type_null:
    rv = JSON_NULL;
    break;
  }

  CAMLreturn (rv);
}

value
virt_builder_json_parser_tree_parse (value stringv)
{
  CAMLparam1 (stringv);
  CAMLlocal1 (rv);
  json_object *tree = NULL;
  json_tokener *tok = NULL;
  enum json_tokener_error err;

  tok = json_tokener_new ();
  json_tokener_set_flags (tok,
                          JSON_TOKENER_STRICT | JSON_TOKENER_VALIDATE_UTF8);
  tree = json_tokener_parse_ex (tok,
                                String_val (stringv),
                                caml_string_length (stringv));
  err = json_tokener_get_error (tok);
  if (err != json_tokener_success) {
    char buf[256];
    snprintf (buf, sizeof buf, "JSON parse error: %s",
              json_tokener_error_desc (err));
    json_tokener_free (tok);
    caml_invalid_argument (buf);
  }
  json_tokener_free (tok);

  rv = convert_json_t (tree, 1);
  json_object_put (tree);

  CAMLreturn (rv);
}
