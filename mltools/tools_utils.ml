(* Common utilities for OCaml tools in libguestfs.
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

open Printf

open Std_utils
open Common_gettext.Gettext
open Getopt.OptionName

(* For this file only, create a reference to prog so that the
 * --program-name option can change the program name in error
 * messages.
 *)
let prog = ref prog

type key_store = {
  keys : (string * key_store_key) list ref;
}
and key_store_key =
  | KeyString of string
  | KeyFileName of string
  | KeyClevis

external c_inspect_decrypt : Guestfs.t -> int64 -> (string * key_store_key) list -> unit = "guestfs_int_mllib_inspect_decrypt"
external c_set_echo_keys : unit -> unit = "guestfs_int_mllib_set_echo_keys" [@@noalloc]
external c_set_keys_from_stdin : unit -> unit = "guestfs_int_mllib_set_keys_from_stdin" [@@noalloc]
external c_rfc3339_date_time_string : unit -> string = "guestfs_int_mllib_rfc3339_date_time_string"

type machine_readable_fn = {
  pr : 'a. ('a, unit, string, unit) format4 -> 'a;
} (* [@@unboxed] *)

type machine_readable_output_type =
  | NoOutput
  | Channel of out_channel
  | File of string
  | Fd of int
let machine_readable_output = ref NoOutput
let machine_readable_channel = ref None
let machine_readable () =
  let chan =
    if !machine_readable_channel = None then (
      let chan =
        match !machine_readable_output with
        | NoOutput -> None
        | Channel chan -> Some chan
        | File f -> Some (open_out f)
        | Fd fd ->
          (* Note that Unix.file_descr is really just an int. *)
          Some (Unix.out_channel_of_descr (Obj.magic fd)) in
      machine_readable_channel := chan
    );
    !machine_readable_channel
  in
  match chan with
  | None -> None
  | Some chan ->
    let pr fs =
      let out s =
        output_string chan s;
        flush chan
      in
      ksprintf out fs
    in
    Some { pr }

(* ANSI terminal colours. *)
let istty chan =
  Unix.isatty (Unix.descr_of_out_channel chan)

let ansi_green ?(chan = stdout) () =
  if colours () || istty chan then output_string chan "\x1b[0;32m"
let ansi_red ?(chan = stdout) () =
  if colours () || istty chan then output_string chan "\x1b[1;31m"
let ansi_blue ?(chan = stdout) () =
  if colours () || istty chan then output_string chan "\x1b[1;34m"
let ansi_magenta ?(chan = stdout) () =
  if colours () || istty chan then output_string chan "\x1b[1;35m"
let ansi_restore ?(chan = stdout) () =
  if colours () || istty chan then output_string chan "\x1b[0m"

let log_as_json msgtype msg =
  match machine_readable () with
  | None -> ()
  | Some { pr } ->
    let json = [
      "message", JSON.String msg;
      "timestamp", JSON.String (c_rfc3339_date_time_string ());
      "type", JSON.String msgtype;
    ] in
    pr "%s\n" (JSON.string_of_doc ~fmt:JSON.Compact json)

(* Timestamped progress messages, used for ordinary messages when not
 * --quiet.
 *)
let start_t = Unix.gettimeofday ()
let message fs =
  let display str =
    log_as_json "message" str;
    if not (quiet ()) then (
      let t = sprintf "%.1f" (Unix.gettimeofday () -. start_t) in
      printf "[%6s] " t;
      ansi_green ();
      printf "%s" str;
      ansi_restore ();
      print_newline ()
    )
  in
  ksprintf display fs

(* Wrap text. *)
type wrap_break_t = WrapEOS | WrapSpace | WrapNL

let rec wrap ?(chan = stdout) ?(indent = 0) str =
  if Std_utils.wrap () || istty chan then
    let len = String.length str in
    _wrap chan indent 0 0 len str
  else (
    output_spaces chan indent;
    output_string chan str
  )

and _wrap chan indent column i len str =
  if i < len then (
    let (j, break) = _wrap_find_next_break i len str in
    let next_column =
      if column + (j-i) >= 76 then (
        output_char chan '\n';
        output_spaces chan indent;
        indent + (j-i) + 1
      )
      else column + (j-i) + 1 in
    output chan (Bytes.of_string str) i (j-i);
    match break with
    | WrapEOS -> ()
    | WrapSpace ->
      output_char chan ' ';
      _wrap chan indent next_column (j+1) len str
    | WrapNL ->
      output_char chan '\n';
      output_spaces chan indent;
      _wrap chan indent indent (j+1) len str
  )

and _wrap_find_next_break i len str =
  if i >= len then (len, WrapEOS)
  else if String.unsafe_get str i = ' ' then (i, WrapSpace)
  else if String.unsafe_get str i = '\n' then (i, WrapNL)
  else _wrap_find_next_break (i+1) len str

(* Error messages etc. *)
let error ?(exit_code = 1) fs =
  let display str =
    log_as_json "error" str;
    let chan = stderr in
    ansi_red ~chan ();
    wrap ~chan (sprintf (f_"%s: error: %s") !prog str);
    if not (verbose () && trace ()) then (
      prerr_newline ();
      prerr_newline ();
      wrap ~chan
           (sprintf (f_"If reporting bugs, run %s with debugging enabled and include the complete output:\n\n  %s -v -x [...]")
                    !prog !prog);
    );
    ansi_restore ~chan ();
    prerr_newline ();
    exit exit_code
  in
  ksprintf display fs

let warning fs =
  let display str =
    log_as_json "warning" str;
    let chan = stdout in
    ansi_blue ~chan ();
    wrap ~chan (sprintf (f_"%s: warning: %s") !prog str);
    ansi_restore ~chan ();
    print_newline ();
  in
  ksprintf display fs

let info fs =
  let display str =
    log_as_json "info" str;
    let chan = stdout in
    ansi_magenta ~chan ();
    wrap ~chan (sprintf (f_"%s: %s") !prog str);
    ansi_restore ~chan ();
    print_newline ();
  in
  ksprintf display fs

(* Print a debug message. *)
let debug fs =
  let display str = if verbose () then prerr_endline str in
  ksprintf display fs

(* Common function to create a new Guestfs handle, with common options
 * (e.g. debug, tracing) already set.
 *)
let open_guestfs ?identifier () =
  let g = new Guestfs.guestfs ~environment:false () in
  g#parse_environment ();
  if trace () then g#set_trace true;
  if verbose () then g#set_verbose true;
  Option.iter g#set_identifier identifier;
  g

(* All the OCaml virt-* programs use this wrapper to catch exceptions
 * and print them nicely.
 *)
let run_main_and_handle_errors main =
  try main ()
  with
  | Unix.Unix_error (code, fname, "") -> (* from a syscall *)
    error (f_"%s: %s") fname (Unix.error_message code)
  | Unix.Unix_error (code, fname, param) -> (* from a syscall *)
    error (f_"%s: %s: %s") fname (Unix.error_message code) param
  | Sys_error msg ->                    (* from a syscall *)
    error (f_"%s") msg
  | Guestfs.Error msg ->                (* from libguestfs *)
    error (f_"libguestfs error: %s") msg
  | Failure msg ->                      (* from failwith/failwithf *)
    error (f_"failure: %s") msg
  | Invalid_argument msg ->             (* probably should never happen *)
    error (f_"internal error: invalid argument: %s") msg
  | Assert_failure (file, line, char) -> (* should never happen *)
    error (f_"internal error: assertion failed at %s, line %d, char %d")
      file line char
  | Not_found ->                        (* should never happen *)
    error (f_"internal error: Not_found exception was thrown")
  | exn ->                              (* something not matched above *)
    error (f_"exception: %s") (Printexc.to_string exn)

(* Print the version number and exit.  Used to implement --version in
 * the OCaml tools.
 *)
let print_version_and_exit () =
  printf "%s %s\n%!" !prog Guestfs_config.package_version_full;
  exit 0

let generated_by =
  sprintf (f_"generated by %s %s") !prog Guestfs_config.package_version_full

let virt_tools_data_dir =
  let dir = lazy (
    try Sys.getenv "VIRT_TOOLS_DATA_DIR"
    with Not_found -> Guestfs_config.datadir // "virt-tools"
  ) in
  fun () -> Lazy.force dir

(* Used by parse_size and parse_resize below. *)
let const_re = PCRE.compile "^([.0-9]+)([bKMG])$"
let plus_const_re = PCRE.compile "^\\+([.0-9]+)([bKMG])$"
let minus_const_re = PCRE.compile "^-([.0-9]+)([bKMG])$"
let percent_re = PCRE.compile "^([.0-9]+)%$"
let plus_percent_re = PCRE.compile "^\\+([.0-9]+)%$"
let minus_percent_re = PCRE.compile "^-([.0-9]+)%$"
let size_scaled f = function
  | "b" -> Int64.of_float f
  | "K" -> Int64.of_float (f *. 1024.)
  | "M" -> Int64.of_float (f *. 1024. *. 1024.)
  | "G" -> Int64.of_float (f *. 1024. *. 1024. *. 1024.)
  | _ -> assert false

(* Parse a size field, eg. "10G". *)
let parse_size field =
  if PCRE.matches const_re field then
    size_scaled (float_of_string (PCRE.sub 1)) (PCRE.sub 2)
  else
    error "%s: cannot parse size field" field

(* Parse a size field, eg. "10G", "+20%" etc.  Used particularly by
 * virt-resize --resize and --resize-force options.
 *)
let parse_resize oldsize field =
  if PCRE.matches const_re field then (
    size_scaled (float_of_string (PCRE.sub 1)) (PCRE.sub 2)
  )
  else if PCRE.matches plus_const_re field then (
    let incr = size_scaled (float_of_string (PCRE.sub 1)) (PCRE.sub 2) in
    oldsize +^ incr
  )
  else if PCRE.matches minus_const_re field then (
    let incr = size_scaled (float_of_string (PCRE.sub 1)) (PCRE.sub 2) in
    oldsize -^ incr
  )
  else if PCRE.matches percent_re field then (
    let percent = Int64.of_float (10. *. float_of_string (PCRE.sub 1)) in
    oldsize *^ percent /^ 1000L
  )
  else if PCRE.matches plus_percent_re field then (
    let percent = Int64.of_float (10. *. float_of_string (PCRE.sub 1)) in
    oldsize +^ oldsize *^ percent /^ 1000L
  )
  else if PCRE.matches minus_percent_re field then (
    let percent = Int64.of_float (10. *. float_of_string (PCRE.sub 1)) in
    oldsize -^ oldsize *^ percent /^ 1000L
  )
  else
    error "%s: cannot parse resize field" field

let human_size i =
  let sign, i = if i < 0L then "-", Int64.neg i else "", i in

  if i < 1024L then
    sprintf "%s%Ld" sign i
  else (
    let f = Int64.to_float i /. 1024. in
    let i = i /^ 1024L in
    if i < 1024L then
      sprintf "%s%.1fK" sign f
    else (
      let f = Int64.to_float i /. 1024. in
      let i = i /^ 1024L in
      if i < 1024L then
        sprintf "%s%.1fM" sign f
      else (
        let f = Int64.to_float i /. 1024. in
        (*let i = i /^ 1024L in*)
        sprintf "%s%.1fG" sign f
      )
    )
  )

type cmdline_options = {
  getopt : Getopt.t;
  ks : key_store;
  debug_gc : bool ref;
}

let create_standard_options argspec ?anon_fun ?(key_opts = false)
      ?(machine_readable = false) ?(program_name = false) usage_msg =
  (* Install an exit hook to check gc consistency for --debug-gc *)
  let debug_gc = ref false in
  let set_debug_gc () = at_exit Gc.compact; debug_gc := true in

  let parse_machine_readable = function
    | None ->
      machine_readable_output := Channel stdout
    | Some fmt ->
      let outtype, outname = String.split ":" fmt in
      if outname = "" then
        error (f_"invalid format string for --machine-readable: %s") fmt;
      (match outtype with
      | "file" -> machine_readable_output := File outname
      | "stream" ->
        let chan =
          match outname with
          | "stdout" -> stdout
          | "stderr" -> stderr
          | n ->
            error (f_"invalid output stream for --machine-readable: %s") fmt in
        machine_readable_output := Channel chan
      | "fd" ->
        (try
          machine_readable_output := Fd (int_of_string outname)
        with Failure _ ->
          error (f_"invalid output fd for --machine-readable: %s") fmt)
      | n ->
        error (f_"invalid output for --machine-readable: %s") fmt
      )
  in
  let ks = {
    keys = ref [];
  } in
  let argspec = ref argspec in
  let add_argspec = List.push_back argspec in

  add_argspec ([ S 'V'; L"version" ], Getopt.Unit print_version_and_exit, s_"Display version and exit");
  add_argspec ([ S 'v'; L"verbose" ], Getopt.Unit set_verbose,  s_"Enable libguestfs debugging messages");
  add_argspec ([ S 'x' ],             Getopt.Unit set_trace,    s_"Enable tracing of libguestfs calls");
  add_argspec ([ L"debug-gc" ],       Getopt.Unit set_debug_gc, Getopt.hidden_option_description);
  add_argspec ([ S 'q'; L"quiet" ],   Getopt.Unit set_quiet,    s_"Don’t print progress messages");
  add_argspec ([ L"color"; L"colors";
                 L"colour"; L"colours" ], Getopt.Unit set_colours, s_"Use ANSI colour sequences even if not tty");
  add_argspec ([ L"wrap" ],           Getopt.Unit set_wrap,     s_"Wrap log messages even if not tty");

  if key_opts then (
    let parse_key_selector arg =
      let parts = String.nsplit ":" arg in
      match parts with
      | [] ->
        error (f_"selector '%s': missing ID") arg
      | [ _ ] ->
        error (f_"selector '%s': missing TYPE") arg
      | [ _; "key" ]
      |  _ :: "key" :: _ :: _ :: _ ->
        error (f_"selector '%s': missing KEY_STRING, or too many fields") arg
      | [ device; "key"; key ] ->
         List.push_back ks.keys (device, KeyString key)
      | [ _; "file" ]
      |  _ :: "file" :: _ :: _ :: _ ->
        error (f_"selector '%s': missing FILENAME, or too many fields") arg
      | [ device; "file"; file ] ->
         List.push_back ks.keys (device, KeyFileName file)
      |  _ :: "clevis" :: _ :: _ ->
        error (f_"selector '%s': too many fields") arg
      | [ device; "clevis" ] ->
         List.push_back ks.keys (device, KeyClevis)
      | _ ->
         error (f_"selector '%s': invalid TYPE") arg
    in

    add_argspec ([ L"echo-keys" ],       Getopt.Unit c_set_echo_keys,       s_"Don’t turn off echo for passphrases");
    add_argspec ([ L"keys-from-stdin" ], Getopt.Unit c_set_keys_from_stdin, s_"Read passphrases from stdin");
    add_argspec ([ L"key" ], Getopt.String (s_"SELECTOR", parse_key_selector), s_"Specify a LUKS key");
  );

  if machine_readable then (
    add_argspec ([ L"machine-readable" ], Getopt.OptString ("format", parse_machine_readable), s_"Make output machine readable");
  );

  if program_name then (
    add_argspec ([ L"program-name" ], Getopt.Set_string ("prog", prog), s_"Set program name");
  );

  let argspec = !argspec in

  let getopt = Getopt.create argspec ?anon_fun usage_msg in
  { getopt; ks; debug_gc }

let external_command_failed help cmd reason =
  let help_prefix = match help with None -> "" | Some str -> str ^ ": " in
  error "%s%s ‘%s’: %s" help_prefix (s_"external command") cmd reason

(* Run an external command, slurp up the output as a list of lines. *)
let external_command ?(echo_cmd = true) ?help cmd =
  if echo_cmd then
    debug "%s" cmd;
  let chan = Unix.open_process_in cmd in
  let lines = ref [] in
  (try while true do lines := input_line chan :: !lines done
   with End_of_file -> ());
  let lines = List.rev !lines in
  let stat = Unix.close_process_in chan in
  (match stat with
  | Unix.WEXITED 0 -> ()
  | Unix.WEXITED i ->
     let reason = sprintf (f_"exited with error %d") i in
     external_command_failed help cmd reason
  | Unix.WSIGNALED i ->
     let reason = sprintf (f_"killed by signal %d") i in
     external_command_failed help cmd reason
  | Unix.WSTOPPED i ->
     let reason = sprintf (f_"stopped by signal %d") i in
     external_command_failed help cmd reason
  );
  lines

let rec run_commands ?(echo_cmd = true) ?help cmds =
  let res = Array.make (List.length cmds) 0 in
  let pids =
    List.mapi (
      fun i (args, stdout_fd, stderr_fd) ->
        let run_res = do_run args ?stdout_fd ?stderr_fd in
        match run_res with
        | Either (pid, app, outfd, errfd) ->
          Some (i, pid, app, outfd, errfd)
        | Or code ->
          res.(i) <- code;
          None
    ) cmds in
  let pids = List.filter_map Fun.id pids in
  let pids = ref pids in
  while !pids <> [] do
    let pid, stat = Unix.waitpid [] 0 in
    let matching_pair, new_pids =
      List.partition (
        fun (_, p, _, _, _) ->
          pid = p
      ) !pids in
    if matching_pair <> [] then (
      let matching_pair = List.hd matching_pair in
      let idx, _, app, outfd, errfd = matching_pair in
      pids := new_pids;
      res.(idx) <- do_teardown help app outfd errfd stat
    );
  done;
  Array.to_list res

and run_command ?(echo_cmd = true) ?help ?stdout_fd ?stderr_fd args =
  let run_res = do_run args ~echo_cmd ?stdout_fd ?stderr_fd in
  match run_res with
  | Either (pid, app, outfd, errfd) ->
    let _, stat = Unix.waitpid [] pid in
    do_teardown help app outfd errfd stat
  | Or code ->
    code

and do_run ?(echo_cmd = true) ?help ?stdout_fd ?stderr_fd args =
  let app = List.hd args in
  let get_fd default = function
    | None ->
      default
    | Some fd ->
      Unix.set_close_on_exec fd;
      fd
  in
  try
    let app = which app in
    let outfd = get_fd Unix.stdout stdout_fd in
    let errfd = get_fd Unix.stderr stderr_fd in
    if echo_cmd then
      debug "%s" (stringify_args args);
    let pid = Unix.create_process app (Array.of_list args) Unix.stdin
                outfd errfd in
    Either (pid, app, stdout_fd, stderr_fd)
  with
  | Executable_not_found _ ->
     debug "%s: executable not found" app;
     Or 127
  | Unix.Unix_error (errcode, fn, _) when errcode = Unix.ENOENT ->
     debug "%s: %s: executable not found" app fn;
     Or 127

and do_teardown help app outfd errfd exitstat =
  Option.iter Unix.close outfd;
  Option.iter Unix.close errfd;
  match exitstat with
  | Unix.WEXITED i ->
     i
  | Unix.WSIGNALED i ->
     let reason = sprintf (f_"killed by signal %d") i in
     external_command_failed help app reason
  | Unix.WSTOPPED i ->
     let reason = sprintf (f_"stopped by signal %d") i in
     external_command_failed help app reason

let shell_command ?(echo_cmd = true) cmd =
  if echo_cmd then
    debug "%s" cmd;
  Sys.command cmd

(* Run uuidgen to return a random UUID. *)
let uuidgen () =
  let lines = external_command "uuidgen -r" in
  assert (List.length lines >= 1);
  let uuid = String.chomp (List.hd lines) in
  if String.length uuid < 10 then assert false; (* sanity check on uuidgen *)
  uuid

(* Using the libguestfs API, recursively remove only files from the
 * given directory.  Useful for cleaning /var/cache etc in sysprep
 * without removing the actual directory structure.  Also if 'dir' is
 * not a directory or doesn't exist, ignore it.
 *
 * The optional filter is used to filter out files which will be
 * removed: files returning true are not removed.
 *
 * XXX Could be faster with a specific API for doing this.
 *)
let rm_rf_only_files (g : Guestfs.guestfs) ?filter dir =
  if g#is_dir dir then (
    let files = Array.map (Filename.concat dir) (g#find dir) in
    let files = Array.to_list files in
    let files = List.filter g#is_file files in
    let files = match filter with
    | None -> files
    | Some f -> List.filter (fun x -> not (f x)) files in
    List.iter g#rm files
  )

let truncate_recursive (g : Guestfs.guestfs) dir =
  let files = Array.map (Filename.concat dir) (g#find dir) in
  let files = Array.to_list files in
  let files = List.filter g#is_file files in
  List.iter g#truncate files

let debug_augeas_errors g =
  if verbose () then (
    try
      let errors = g#aug_match "/augeas/files//error" in
      let errors = Array.to_list errors in
      let map =
        List.fold_left (
          fun map error ->
            let detail_paths = g#aug_match (error ^ "//*") in
            let detail_paths = Array.to_list detail_paths in
            List.fold_left (
              fun map path ->
                (* path is "/augeas/files/<filename>/error/<field>".  Put
                 * <filename>, <field> and the value of this Augeas field
                 * into a map.
                 *)
                let i = String.find path "/error/" in
                assert (i >= 0);
                let filename = String.sub path 13 (i-13) in
                let field =
                  String.sub path (i+7) (String.length path - (i+7)) in

                let detail = g#aug_get path in

                let fmap : string StringMap.t =
                  try StringMap.find filename map
                  with Not_found -> StringMap.empty in
                let fmap = StringMap.add field detail fmap in
                StringMap.add filename fmap map
            ) map detail_paths
        ) StringMap.empty errors in

      let filenames = StringMap.keys map in
      let filenames = List.sort compare filenames in

      List.iter (
        fun filename ->
          eprintf "augeas failed to parse %s:\n" filename;
          let fmap = StringMap.find filename map in
          (try
            let msg = StringMap.find "message" fmap in
            eprintf " error \"%s\"" msg
          with Not_found -> ()
          );
          (try
            let line = StringMap.find "line" fmap
            and char = StringMap.find "char" fmap in
            eprintf " at line %s char %s" line char
          with Not_found -> ()
          );
          (try
            let lens = StringMap.find "lens" fmap in
            eprintf " in lens %s" lens
          with Not_found -> ()
          );
          eprintf "\n"
      ) filenames;

      flush stderr
    with
      Guestfs.Error msg -> eprintf "%s: augeas: %s (ignored)\n" !prog msg
  )

(* Detect type of a file. *)
let detect_file_type filename =
  with_open_in filename (
    fun chan ->
      let get start size =
        try
          seek_in chan start;
          let b = Bytes.create size in
          really_input chan b 0 size;
          Some (Bytes.to_string b)
        with End_of_file | Invalid_argument _ -> None
      in
      if get 0 6 = Some "\2537zXZ\000" then `XZ
      else if get 0 4 = Some "PK\003\004" then `Zip
      else if get 0 4 = Some "PK\005\006" then `Zip
      else if get 0 4 = Some "PK\007\008" then `Zip
      else if get 257 6 = Some "ustar\000" then `Tar
      else if get 257 8 = Some "ustar\x20\x20\000" then `Tar
      else if get 0 2 = Some "\x1f\x8b" then `GZip
      else `Unknown
  )

let is_partition dev =
  try
    if not (is_block_device dev) then false
    else (
      let rdev = (Unix.stat dev).Unix.st_rdev in
      let major = Unix_utils.Dev_t.major rdev in
      let minor = Unix_utils.Dev_t.minor rdev in
      let path = sprintf "/sys/dev/block/%d:%d/partition" major minor in
      Unix.access path [Unix.F_OK];
      true
    )
  with Unix.Unix_error _ -> false

let inspect_mount_root g ?mount_opts_fn root =
  let mps = g#inspect_get_mountpoints root in
  let cmp (a,_) (b,_) =
    compare (String.length a) (String.length b) in
  let mps = List.sort cmp mps in
  List.iter (
    fun (mp, dev) ->
      let mountfn =
        match mount_opts_fn with
        | Some fn -> g#mount_options (fn mp)
        | None -> g#mount in
      try mountfn dev mp
      with Guestfs.Error msg -> warning (f_"%s (ignored)") msg
  ) mps

let inspect_mount_root_ro =
  inspect_mount_root ~mount_opts_fn:(fun _ -> "ro")

let is_btrfs_subvolume g fs =
  try
    ignore (g#mountable_subvolume fs); true
  with Guestfs.Error msg as exn ->
    if g#last_errno () = Guestfs.Errno.errno_EINVAL then false
    else raise exn

let key_store_requires_network ks =
  List.exists (function
               | _, KeyClevis -> true
               | _ -> false) !(ks.keys)

let inspect_decrypt g ks =
  (* Note we pass original 'g' even though it is not used by the
   * callee.  This is so that 'g' is kept as a root on the stack, and
   * so cannot be garbage collected while we are in the c_inspect_decrypt
   * function.
   *)
  c_inspect_decrypt g#ocaml_handle (Guestfs.c_pointer g#ocaml_handle)
    !(ks.keys)

let with_timeout op timeout ?(sleep = 2) fn =
  let start_t = Unix.gettimeofday () in
  let rec loop () =
    if Unix.gettimeofday () -. start_t > float_of_int timeout then
      error (f_"%s: operation timed out") op;

    match fn () with
    | Some r -> r
    | None ->
       Unix.sleep sleep;
       loop ()
  in
  loop ()

let run_in_guest_command g root ?logfile ?incompatible_fn cmd =
  (* Is the host_cpu compatible with the guest arch?  ie. Can we
   * run commands in this guest?
   *)
  let guest_os = g#inspect_get_type root in
  let guest_os_compatible =
    String.starts_with "linux" Guestfs_config.host_os &&
    guest_os = "linux" in
  let guest_arch = g#inspect_get_arch root in
  let guest_arch_compatible = guest_arch_compatible guest_arch in
  if not guest_os_compatible || not guest_arch_compatible then (
    match incompatible_fn with
    | None ->
       error (f_"host (%s/%s) and guest (%s/%s) are not compatible, \
                 so you cannot use command line options that involve \
                 running commands in the guest.  Use --firstboot scripts \
                 instead.")
         Guestfs_config.host_os Guestfs_config.host_cpu
         guest_os guest_arch
    | Some fn -> fn ()
  )
  else (
    (* Add a prologue to the scripts:
     * - Pass environment variables through from the host.
     * - Optionally send stdout and stderr to a log file so we capture
     *   all output in error messages.
     * - Use setarch when running x86_64 host + i686 guest.
     *)
    let env_vars =
      List.filter_map (
        fun name ->
          try Some (sprintf "export %s=%s" name (quote (Sys.getenv name)))
          with Not_found -> None
      ) [ "http_proxy"; "https_proxy"; "ftp_proxy"; "no_proxy" ] in
    let env_vars = String.concat "\n" env_vars ^ "\n" in

    let cmd =
      match Guestfs_config.host_cpu, guest_arch with
      | "x86_64", ("i386"|"i486"|"i586"|"i686") ->
        sprintf "setarch i686 <<\"__EOCMD\"
%s
__EOCMD
" cmd
      | _ -> cmd in

    let logfile_redirect =
      match logfile with
      | None -> ""
      | Some logfile -> sprintf "exec >>%s 2>&1" (quote logfile) in

    let cmd = sprintf "\
%s
%s
%s
" (logfile_redirect) env_vars cmd in

    debug "running command:\n%s" cmd;
    ignore (g#sh cmd)
  )
