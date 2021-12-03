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
open Common_gettext.Gettext

open Unix
open Printf

(* List of files to unlink. *)
let files = ref []

(* List of directories to remove. *)
let rmdirs = ref []

(* List of PIDs to kill. *)
let kills = ref []

(* List of functions to call. *)
let fns = ref []

(* Make sure the actions are performed only once. *)
let done_actions = ref false

(* Perform the exit actions. *)
let do_actions () =
  if not !done_actions then (
    List.iter (fun f -> f ()) !fns;
    List.iter (fun (signal, pid) -> kill pid signal) !kills;
    List.iter (
      fun dir ->
        let cmd = sprintf "rm -rf %s" (Filename.quote dir) in
        ignore (Tools_utils.shell_command cmd)
    ) !rmdirs;
    List.iter (fun file -> try Unix.unlink file with _ -> ()) !files;
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
  List.push_front fn fns

let unlink filename =
  register ();
  List.push_front filename files

let rmdir dir =
  register ();
  List.push_front dir rmdirs

let kill ?(signal = Sys.sigterm) pid =
  register ();
  List.push_front (signal, pid) kills
