(* Common way to handle actions on exit.
 * Copyright (C) 2010-2019 Red Hat Inc.
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

open Std_utils
open Tools_utils
open Common_gettext.Gettext

open Unix
open Printf

type action =
  | Unlink of string     (* filename *)
  | Rm_rf of string      (* directory *)
  | Kill of int * int    (* signal, pid *)
  | Fn of (unit -> unit) (* generic function *)

(* List of actions. *)
let actions = ref []

(* Perform a single exit action, printing any exception but
 * otherwise ignoring failures.
 *)
let do_action action =
  try
    match action with
    | Unlink file -> Unix.unlink file
    | Rm_rf dir ->
       let cmd = sprintf "rm -rf -- %s" (Filename.quote dir) in
       ignore (Tools_utils.shell_command cmd)
    | Kill (signal, pid) ->
       kill pid signal
    | Fn f -> f ()
  with exn -> debug "%s" (Printexc.to_string exn)

(* Make sure the actions are performed only once. *)
let done_actions = ref false

(* Perform the exit actions. *)
let do_actions () =
  if not !done_actions then (
    List.iter do_action !actions
  );
  done_actions := true

(* False until at least one function is called.  Avoids registering
 * the signal and at_exit handlers unnnecessarily.
 *)
let registered = ref false

(* Register signal and at_exit handlers. *)
let register () =
  if not !registered then (
    List.iter (
      fun (signal, name) ->
        let handler _ =
          (* Try to get a final message out.  This is helpful
           * when debugging so we can tell if a program was killed
           * or segfaulted.
           *)
          eprintf (f_"%s: Exiting on signal %s\n%!") prog name;
          (* Do the cleanup actions. *)
          do_actions ();
          (* Call _exit instead of exit because the OCaml exit calls
           * C exit which is probably not safe from a signal handler
           * especially if we forked.
           *)
          Unix_utils.Exit._exit 1
        in
        ignore (Sys.signal signal (Sys.Signal_handle handler))
    ) [ Sys.sigint, "SIGINT";
        Sys.sigquit, "SIGQUIT";
        Sys.sigterm, "SIGTERM";
        Sys.sighup, "SIGHUP" ];

    (* Register the at_exit handler. *)
    at_exit do_actions
  );
  registered := true

let f fn =
  register ();
  List.push_front (Fn fn) actions

let unlink filename =
  register ();
  List.push_front (Unlink filename) actions

let rm_rf dir =
  register ();
  List.push_front (Rm_rf dir) actions

let kill ?(signal = Sys.sigterm) pid =
  register ();
  List.push_front (Kill (signal, pid)) actions
