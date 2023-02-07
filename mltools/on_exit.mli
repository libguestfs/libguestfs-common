(* Common way to handle actions on exit.
 * Copyright (C) 2010-2023 Red Hat Inc.
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

(** This module allows you to register actions which
    are mostly guaranteed to happen when the process
    exits, even if it exits with a signal.  Note that
    segfaults, kill -9, etc. will not run these actions.

    The main problem with the {!Stdlib.at_exit} function
    is that it is not called if the program exits with
    a signal.  A lesser problem is that it's hard to
    use for common cases such as deleting a temp file or
    killing another process, so we provide simple
    wrappers for those common actions here.

    Actions can be ordered by setting the optional [?prio]
    parameter in the range 0..9999.  By default actions
    have priority 5000.  Lower numbered actions run first.
    Higher numbered actions run last.  So to have an action
    run at the very end before exit you might use [~prio:9999]

    Note this module registers signal handlers for
    SIGINT, SIGQUIT, SIGTERM and SIGHUP.  This means
    that any program that links with mltools.cmxa
    will automatically have signal handlers pointing
    to an internal function within this module.  To
    register your own signal handler function to be
    called instead, you have to call {!register}
    before using the ordinary {!Sys.signal} functions.
    Your cleanup action might no longer run unless the
    program calls {!Stdlib.exit}. *)

val f : ?prio:int -> (unit -> unit) -> unit
(** Register a function [f] which runs when the program exits.
    Similar to [Stdlib.at_exit] but also runs if the program is
    killed with a signal that we can catch.

    [?prio] is the priority, default 5000.  See the description above. *)

val unlink : ?prio:int -> string -> unit
(** Unlink a single temporary file on exit. *)

val rm_rf : ?prio:int -> string -> unit
(** Recursively remove a temporary directory on exit (using [rm -rf]). *)

val kill : ?prio:int -> ?signal:int -> int -> unit
(** Kill [PID] on exit.  The signal sent defaults to [Sys.sigterm].

    Use this with care since you can end up unintentionally killing
    another process if [PID] goes away or doesn't exist before the
    program exits. *)

val register : unit -> unit
(** Force this module to register its at_exit function and signal
    handlers now.  You do {!i not} normally need to call this.
    Calling the functions above implicitly calls register.  However
    you might need to call it if you want to register your own
    signal handler.  See above description for why and use with care.
    Note that this is safe if at_exit and signal handlers have
    already been registered - it does nothing in that case. *)
