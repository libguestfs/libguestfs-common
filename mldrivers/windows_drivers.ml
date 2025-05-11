(* virt-drivers
 * Copyright (C) 2009-2025 Red Hat Inc.
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

(* Detect which drivers are installed in the Windows Registry. *)

open Printf
open Scanf

open Std_utils
open Tools_utils
open Common_gettext.Gettext

module G = Guestfs

type driver = {
  name : string;                (** Driver name, eg. ["usbport"] *)
  hwassoc : hardware list;      (** Associated list of hardware *)
}
and hardware =
| PCI of pci_device
| HID of hid_device
| USB of usb_device
| Other of string list          (** Anything that could not be parsed. *)
and pci_device = {
  pci_class : int64 option;
  pci_vendor : int64 option;
  pci_device : int64 option;
  pci_subsys : int64 option;
  pci_rev : int64 option;
}
and hid_device = {
  hid_vendor : int64 option;
  hid_product : int64 option;
  hid_rev : int64 option;
  hid_col : int64 option;
  hid_multi : int64 option;
}
and usb_device = {
  usb_vendor : int64 option;
  usb_product : int64 option;
  usb_rev : int64 option;
  usb_multi : int64 option;
}

let re_inf = PCRE.compile ~caseless:true "^(.*)\\.inf$"

let re_pci_cc = PCRE.compile ~caseless:true "^cc_([[:xdigit:]]{4,6})$"
let re_pci_ven = PCRE.compile ~caseless:true "^ven_([[:xdigit:]]{4})$"
let re_pci_dev = PCRE.compile ~caseless:true "^dev_([[:xdigit:]]{4})$"
let re_pci_subsys = PCRE.compile ~caseless:true "^subsys_([[:xdigit:]]{8})$"
let re_pci_rev = PCRE.compile ~caseless:true "^rev_([[:xdigit:]]{2})$"

let re_hid_vid = PCRE.compile ~caseless:true "^vid_([[:xdigit:]]{4})$"
let re_hid_pid = PCRE.compile ~caseless:true "^pid_([[:xdigit:]]{4})$"
let re_hid_rev = PCRE.compile ~caseless:true "^rev_([[:xdigit:]]{2})$"
let re_hid_col = PCRE.compile ~caseless:true "^col([[:xdigit:]]{2})$"
let re_hid_multi = PCRE.compile ~caseless:true "^mi_([[:xdigit:]]{2})$"

let re_usb_vid = PCRE.compile ~caseless:true "^vid_([[:xdigit:]]{4})$"
let re_usb_pid = PCRE.compile ~caseless:true "^pid_([[:xdigit:]]{4})$"
let re_usb_rev = PCRE.compile ~caseless:true "^rev_([[:xdigit:]]{2})$"
let re_usb_multi = PCRE.compile ~caseless:true "^mi_([[:xdigit:]]{2})$"

let rec detect_drivers (g : G.guestfs) root =
  assert (g#inspect_get_type root = "windows");

  let windows_system_hive = g#inspect_get_windows_system_hive root in
  let drivers =
    Registry.with_hive_readonly g windows_system_hive (
      fun reg ->
        let path = [ "DriverDatabase"; "DeviceIds" ] in
        let deviceids_node =
          match Registry.get_node reg path with
          | Some node -> node
          | None ->
             error (f_"could not find registry entry \
                       HKEY_LOCAL_MACHINE\\SYSTEM\\DriverDatabase\\DeviceIds \
                       in this Windows guest.  It may be either a very old \
                       or very new version of Windows \
                       that we cannot process.") in

        (*             inf_name   path          node *)
        let children : (string * (string list * int64)) list =
          get_inf_nodes reg deviceids_node in

        let children =
          List.map (fun (name, (path, node)) ->
              String.lowercase_ascii name, (path, node)) children in

        (* Group by inf_name. *)
        let children = List.sort compare children in
        let children : (string * (string list * int64) list) list=
          List.group_by children in

        (* Convert to a final list of drivers. *)
        List.map (
          fun (inf_name, hwassoc) ->
            let hwassoc = List.map path_to_hardware hwassoc in
            { name = inf_name; hwassoc }
        ) children
  ) in
  drivers

(* Find recursively all child nodes containing a key
 * "<foo>.inf" = hex(3):01,ff,00,00
 *
 * Returns the list of [inf_name * (path * node)] where
 * [inf_name] is the <foo> part of "<foo>.inf",
 * [path] is the list of strings (node names) leading to this node,
 * [node] is the hivex node number of the child node.
 *)
and get_inf_nodes reg root =
  let nodes : (string list * int64) list = find_all_nodes reg root in
  List.filter_map (
    fun (path, h) ->
      match is_inf_node reg h with
      | None -> None
      | Some inf_name -> Some (inf_name, (path, h))
  ) nodes

and find_all_nodes ((g, _) as reg) node =
  (* Find all children of [node]. *)
  let children = g#hivex_node_children node in
  let children = Array.to_list children in
  let children =
    List.map (fun { G.hivex_node_h = h } -> [ g#hivex_node_name h ], h)
      children in

  (* Add any grandchild nodes below these children. *)
  let grandchildren =
    List.map (
      fun (child_path, child) ->
        let nodes = find_all_nodes reg child in
        (* Need to prefix the path returned with the child path. *)
        List.map (fun (path, h) -> child_path @ path, h) nodes
    ) children in

  children @ List.flatten grandchildren

(* Does any value under the node satisfy is_inf_value below? *)
and is_inf_node ((g, _) as reg) h =
  (* Get the values in the registry key. *)
  let values = g#hivex_node_values h in
  let values = Array.to_list values in
  let rec loop = function
    | [] -> None
    | { G.hivex_value_h = v } :: values ->
       match is_inf_value reg v with
       | None -> loop values
       | Some inf_name -> Some inf_name
  in
  loop values

(* Does the key name end in "<name>.inf" (case insensitive), and have
 * an associated value which is data type 3, value 01 ff 00 00?
 * Returns None if no, or Some name if yes.
 *)
and is_inf_value (g, _) v =
  let typ = g#hivex_value_type v in
  if typ <> 3L then None
  else (
    let key = g#hivex_value_key v in
    if not (PCRE.matches re_inf key) then None
    else (
      let data = g#hivex_value_value v in
      if data <> "\001\xff\000\000" then None
      else Some (PCRE.sub 1) (* Return name of .inf file from re_inf above. *)
    )
  )

(* Convert the \DeviceIds\... path to a hardware type, where possible. *)
and path_to_hardware (path, _) =
  match path with
  | pci :: path when String.lowercase_ascii pci = "pci" ->
     pci_to_hardware path
  | hid :: path when String.lowercase_ascii hid = "hid" ->
     hid_to_hardware path
  | usb :: path when String.lowercase_ascii usb = "usb" ->
     usb_to_hardware path
  | _ -> Other path

and pci_to_hardware = function
  | keys :: _ ->
     let keys = String.nsplit "&" keys in
     let empty = { pci_class = None; pci_vendor = None; pci_device = None;
                   pci_subsys = None; pci_rev = None } in
     let f pci key =
       if PCRE.matches re_pci_cc key then
         { pci with pci_class = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_pci_ven key then
         { pci with pci_vendor = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_pci_dev key then
         { pci with pci_device = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_pci_subsys key then
         { pci with pci_subsys = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_pci_rev key then
         { pci with pci_rev = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else
         pci
     in
     PCI (List.fold_left f empty keys)
  | path -> Other ("PCI" :: path)

and hid_to_hardware = function
  | keys :: _ ->
     let keys = String.nsplit "&" keys in
     let empty = { hid_vendor = None; hid_product = None;
                   hid_rev = None; hid_col = None; hid_multi = None } in
     let f hid key =
       if PCRE.matches re_hid_vid key then
         { hid with hid_vendor = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_hid_pid key then
         { hid with hid_product = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_hid_rev key then
         { hid with hid_rev = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_hid_col key then
         { hid with hid_col = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_hid_multi key then
         { hid with hid_multi = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else
         hid
     in
     HID (List.fold_left f empty keys)

  | path -> Other ("HID" :: path)

and usb_to_hardware = function
  | keys :: _ ->
     let keys = String.nsplit "&" keys in
     let empty = { usb_vendor = None; usb_product = None;
                   usb_rev = None; usb_multi = None } in
     let f usb key =
       if PCRE.matches re_usb_vid key then
         { usb with usb_vendor = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_usb_pid key then
         { usb with usb_product = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_usb_rev key then
         { usb with usb_rev = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else if PCRE.matches re_usb_multi key then
         { usb with usb_multi = Some (sscanf (PCRE.sub 1) "%Lx" Fun.id) }
       else
         usb
     in
     USB (List.fold_left f empty keys)

  | path -> Other ("USB" :: path)
