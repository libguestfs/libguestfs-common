(* virt-drivers
 * Copyright (C) 2009-2023 Red Hat Inc.
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

(** Detect which drivers are installed in the Windows Registry. *)

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

val detect_drivers : Guestfs.guestfs -> string -> driver list
(** [detect_kernels g root] detects the drivers installed in
    the Windows guest, returning their names and the hardware
    associations.

    The information is retrieved from the
    [HKEY_LOCAL_MACHINE\SYSTEM\DriverDatabase\DeviceIds\]
    key in the system registry. *)
