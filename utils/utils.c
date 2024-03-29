/* libguestfs
 * Copyright (C) 2009-2019 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * Utility functions used by the library, tools and language bindings.
 *
 * These functions I<must not> call internal library functions
 * such as C<safe_*>, C<error> or C<perrorf>, or any C<guestfs_int_*>.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libintl.h>

/* NB: MUST NOT require linking to gnulib, because that will break the
 * Python 'sdist' which includes a copy of this file.  It's OK to
 * include "c-ctype.h" and "ignore-value.h" here (since it is a header
 * only with no other code), but we also had to copy these files to
 * the Python sdist.
 */
#include "c-ctype.h"
#include "ignore-value.h"

/* NB: MUST NOT include "guestfs-internal.h". */
#include "guestfs-utils.h"

/**
 * Replace every instance of C<s1> appearing in C<str> with C<s2>.  A
 * newly allocated string is returned which must be freed by the
 * caller.  If allocation fails this can return C<NULL>.
 *
 * For example:
 *
 *  replace_string ("abcabb", "ab", "a");
 *
 * would return C<"acab">.
 */
char *
guestfs_int_replace_string (const char *str, const char *s1, const char *s2)
{
  const size_t len = strlen (str), s1len = strlen (s1), s2len = strlen (s2);
  size_t i, n;
  char *ret;

  /* Count the size of the final string. */
  n = 0;
  for (i = 0; i < len; ++i) {
    if (strncmp (&str[i], s1, s1len) == 0)
      n += s2len;
    else
      n++;
  }

  ret = malloc (n+1);
  if (ret == NULL)
    return NULL;

  n = 0;
  for (i = 0; i < len; ++i) {
    if (strncmp (&str[i], s1, s1len) == 0) {
      strcpy (&ret[n], s2);
      n += s2len;
    }
    else {
      ret[n] = str[i];
      n++;
    }
  }
  ret[n] = '\0';

  return ret;
}

/**
 * Translate a wait/system exit status into a printable string.
 */
char *
guestfs_int_exit_status_to_string (int status, const char *cmd_name,
				   char *buffer, size_t buflen)
{
  if (WIFEXITED (status)) {
    if (WEXITSTATUS (status) == 0)
      snprintf (buffer, buflen, _("%s exited successfully"),
                cmd_name);
    else
      snprintf (buffer, buflen, _("%s exited with error status %d"),
                cmd_name, WEXITSTATUS (status));
  }
  else if (WIFSIGNALED (status)) {
    snprintf (buffer, buflen, _("%s killed by signal %d (%s)"),
              cmd_name, WTERMSIG (status), strsignal (WTERMSIG (status)));
  }
  else if (WIFSTOPPED (status)) {
    snprintf (buffer, buflen, _("%s stopped by signal %d (%s)"),
              cmd_name, WSTOPSIG (status), strsignal (WSTOPSIG (status)));
  }
  else {
    snprintf (buffer, buflen, _("%s exited for an unknown reason (status %d)"),
              cmd_name, status);
  }

  return buffer;
}

/**
 * Return a random string of characters.
 *
 * Notes:
 *
 * =over 4
 *
 * =item *
 *
 * The C<ret> buffer must have length C<len+1> in order to store the
 * final C<\0> character.
 *
 * =item *
 *
 * There is about 5 bits of randomness per output character (so about
 * C<5*len> bits of randomness in the resulting string).
 *
 * =back
 */
int
guestfs_int_random_string (char *ret, size_t len)
{
  int fd;
  size_t i;
  unsigned char c;
  int saved_errno;

  fd = open ("/dev/urandom", O_RDONLY|O_CLOEXEC);
  if (fd == -1)
    return -1;

  for (i = 0; i < len; ++i) {
    if (read (fd, &c, 1) != 1) {
      saved_errno = errno;
      close (fd);
      errno = saved_errno;
      return -1;
    }
    /* Do not change this! */
    ret[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[c % 36];
  }
  ret[len] = '\0';

  if (close (fd) == -1)
    return -1;

  return 0;
}

/**
 * This turns a drive index (eg. C<27>) into a drive name
 * (eg. C<"ab">).
 *
 * Drive indexes count from C<0>.  The return buffer has to be large
 * enough for the resulting string, and the returned pointer points to
 * the *end* of the string.
 *
 * L<https://rwmj.wordpress.com/2011/01/09/how-are-linux-drives-named-beyond-drive-26-devsdz/>
 */
char *
guestfs_int_drive_name (size_t index, char *ret)
{
  if (index >= 26)
    ret = guestfs_int_drive_name (index/26 - 1, ret);
  index %= 26;
  *ret++ = 'a' + index;
  *ret = '\0';
  return ret;
}

/**
 * The opposite of C<guestfs_int_drive_name>.  Take a string like
 * C<"ab"> and return the index (eg C<27>).
 *
 * Note that you must remove any prefix such as C<"hd">, C<"sd"> etc,
 * or any partition number before calling the function.
 */
ssize_t
guestfs_int_drive_index (const char *name)
{
  ssize_t r = 0;

  while (*name) {
    if (*name >= 'a' && *name <= 'z')
      r = 26*r + (*name - 'a' + 1);
    else
      return -1;
    name++;
  }

  return r-1;
}

/**
 * Similar to C<Tcl_GetBoolean>.
 */
int
guestfs_int_is_true (const char *str)
{
  if (STREQ (str, "1") ||
      STRCASEEQ (str, "true") ||
      STRCASEEQ (str, "t") ||
      STRCASEEQ (str, "yes") ||
      STRCASEEQ (str, "y") ||
      STRCASEEQ (str, "on"))
    return 1;

  if (STREQ (str, "0") ||
      STRCASEEQ (str, "false") ||
      STRCASEEQ (str, "f") ||
      STRCASEEQ (str, "no") ||
      STRCASEEQ (str, "n") ||
      STRCASEEQ (str, "off"))
    return 0;

  return -1;
}

/**
 * Check a string for validity, that it contains only certain
 * characters, and minimum and maximum length.  This function is
 * usually wrapped in a VALID_* macro, see F<lib/drives.c> for an
 * example.
 *
 * C<str> is the string to check.
 *
 * C<min_length> and C<max_length> are the minimum and maximum
 * length checks.  C<0> means no check.
 *
 * The flags control:
 *
 * =over 4
 *
 * =item C<VALID_FLAG_ALPHA>
 *
 * 7-bit ASCII-only alphabetic characters are permitted.
 *
 * =item C<VALID_FLAG_DIGIT>
 *
 * 7-bit ASCII-only digits are permitted.
 *
 * =back
 *
 * C<extra> is a set of extra characters permitted, in addition
 * to alphabetic and/or digits.  (C<extra = NULL> for no extra).
 *
 * Returns boolean C<true> if the string is valid (passes all the
 * tests), or C<false> if not.
 */
bool
guestfs_int_string_is_valid (const char *str,
                             size_t min_length, size_t max_length,
                             int flags, const char *extra)
{
  size_t i, len = strlen (str);

  if ((min_length > 0 && len < min_length) ||
      (max_length > 0 && len > max_length))
    return false;

  for (i = 0; i < len; ++i) {
    bool valid_char;

    valid_char =
      ((flags & VALID_FLAG_ALPHA) && c_isalpha (str[i])) ||
      ((flags & VALID_FLAG_DIGIT) && c_isdigit (str[i])) ||
      (extra && strchr (extra, str[i]));

    if (!valid_char) return false;
  }

  return true;
}

#if 0 /* not used yet */
/**
 * Hint that we will read or write the file descriptor normally.
 *
 * On Linux, this clears the C<FMODE_RANDOM> flag on the file [see
 * below] and sets the per-file number of readahead pages to equal the
 * block device readahead setting.
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_normal (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_NORMAL)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_NORMAL));
#endif
}
#endif

/**
 * Hint that we will read or write the file descriptor sequentially.
 *
 * On Linux, this clears the C<FMODE_RANDOM> flag on the file [see
 * below] and sets the per-file number of readahead pages to twice the
 * block device readahead setting.
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_sequential (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_SEQUENTIAL)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_SEQUENTIAL));
#endif
}

/**
 * Hint that we will read or write the file descriptor randomly.
 *
 * On Linux, this sets the C<FMODE_RANDOM> flag on the file.  The
 * effect of this flag is to:
 *
 * =over 4
 *
 * =item *
 *
 * Disable normal sequential file readahead.
 *
 * =item *
 *
 * If any read of the file is done which misses in the page cache, 2MB
 * are read into the page cache.  [I think - I'm not sure I totally
 * understand what this is doing]
 *
 * =back
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_random (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_RANDOM)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_RANDOM));
#endif
}

/**
 * Hint that we will access the data only once.
 *
 * On Linux, this does nothing.
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_noreuse (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_NOREUSE)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_NOREUSE));
#endif
}

#if 0 /* not used yet */
/**
 * Hint that we will not access the data in the near future.
 *
 * On Linux, this immediately writes out any dirty pages in the page
 * cache and then invalidates (drops) all pages associated with this
 * file from the page cache.  Apparently it does this even if the file
 * is opened or being used by other processes.  This setting is not
 * persistent; if you subsequently read the file it will be cached in
 * the page cache as normal.
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_dontneed (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_DONTNEED)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_DONTNEED));
#endif
}
#endif

#if 0 /* not used yet */
/**
 * Hint that we will access the data in the near future.
 *
 * On Linux, this immediately reads the whole file into the page
 * cache.  This setting is not persistent; subsequently pages may be
 * dropped from the page cache as normal.
 *
 * It's OK to call this on a non-file since we ignore failure as it is
 * only a hint.
 */
void
guestfs_int_fadvise_willneed (int fd)
{
#if defined(HAVE_POSIX_FADVISE) && defined(POSIX_FADV_WILLNEED)
  /* It's not clear from the man page, but the 'advice' parameter is
   * NOT a bitmask.  You can only pass one parameter with each call.
   */
  ignore_value (posix_fadvise (fd, 0, 0, POSIX_FADV_WILLNEED));
#endif
}
#endif

/**
 * Unquote a shell-quoted string.
 *
 * Augeas passes strings to us which may be quoted, eg. if they come
 * from files in F</etc/sysconfig>.  This function can do simple
 * unquoting of these strings.
 *
 * Note this function does not do variable substitution, since that is
 * impossible without knowing the file context and indeed the
 * environment under which the shell script is run.  Configuration
 * files should not use complex quoting.
 *
 * C<str> is the input string from Augeas, a string that may be
 * single- or double-quoted or may not be quoted.  The returned string
 * is unquoted, and must be freed by the caller.  C<NULL> is returned
 * on error and C<errno> is set accordingly.
 *
 * For information on double-quoting in bash, see
 * L<https://www.gnu.org/software/bash/manual/html_node/Double-Quotes.html>
 */
char *
guestfs_int_shell_unquote (const char *str)
{
  size_t len = strlen (str);
  char *ret;

  if (len >= 2) {
    if (str[0] == '\'' && str[len-1] == '\'') {
                                /* single quoting */
      ret = strndup (&str[1], len-2);
      if (ret == NULL)
        return NULL;
      return ret;
    }
    else if (str[0] == '"' && str[len-1] == '"') {
                                /* double quoting */
      size_t i, j;

      ret = malloc (len + 1);   /* strings always get smaller */
      if (ret == NULL)
        return NULL;

      for (i = 1, j = 0; i < len-1 /* ignore final quote */; ++i, ++j) {
        if (i < len-2 /* ignore final char before final quote */ &&
            str[i] == '\\' &&
            (str[i+1] == '$' || str[i+1] == '`' || str[i+1] == '"' ||
             str[i+1] == '\\' || str[i+1] == '\n'))
          ++i;
        ret[j] = str[i];
      }

      ret[j] = '\0';

      return ret;
    }
  }

  return strdup (str);
}

/* In the libguestfs API, modes returned by lstat and friends are
 * defined to contain Linux ABI values.  However since the "current
 * operating system" might not be Linux, we have to hard-code those
 * numbers in the functions below.
 */

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a regular file.
 */
int
guestfs_int_is_reg (int64_t mode)
{
  return (mode & 0170000) == 0100000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a directory.
 */
int
guestfs_int_is_dir (int64_t mode)
{
  return (mode & 0170000) == 0040000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a char device.
 */
int
guestfs_int_is_chr (int64_t mode)
{
  return (mode & 0170000) == 0020000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a block device.
 */
int
guestfs_int_is_blk (int64_t mode)
{
  return (mode & 0170000) == 0060000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a named pipe (FIFO).
 */
int
guestfs_int_is_fifo (int64_t mode)
{
  return (mode & 0170000) == 0010000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a symbolic link.
 */
int
guestfs_int_is_lnk (int64_t mode)
{
  return (mode & 0170000) == 0120000;
}

/**
 * Return true if the C<guestfs_statns> or C<guestfs_lstatns>
 * C<st_mode> field represents a Unix domain socket.
 */
int
guestfs_int_is_sock (int64_t mode)
{
  return (mode & 0170000) == 0140000;
}

/**
 * Concatenate C<dir> and C<name> to create a path.  This correctly
 * handles the case of concatenating C<"/" + "filename"> as well
 * as C<"/dir" + "filename">.  C<name> may be C<NULL>.
 *
 * The caller must free the returned path.
 *
 * This function sets C<errno> and returns C<NULL> on error.
 */
char *
guestfs_int_full_path (const char *dir, const char *name)
{
  int r;
  char *path;
  int len;

  len = strlen (dir);
  if (len > 0 && dir[len - 1] == '/')
    --len;

  if (STREQ (dir, "/"))
    r = asprintf (&path, "/%s", name ? name : "");
  else if (name)
    r = asprintf (&path, "%.*s/%s", len, dir, name);
  else
    r = asprintf (&path, "%.*s", len, dir);

  if (r == -1)
    return NULL;

  return path;
}

/**
 * Hexdump a block of memory to C<FILE *>, used for debugging.
 */
void
guestfs_int_hexdump (const void *data, size_t len, FILE *fp)
{
  size_t i, j;

  for (i = 0; i < len; i += 16) {
    fprintf (fp, "%04zx: ", i);
    for (j = i; j < MIN (i+16, len); ++j)
      fprintf (fp, "%02x ", ((const unsigned char *)data)[j]);
    for (; j < i+16; ++j)
      fprintf (fp, "   ");
    fprintf (fp, "|");
    for (j = i; j < MIN (i+16, len); ++j)
      if (c_isprint (((const char *)data)[j]))
	fprintf (fp, "%c", ((const char *)data)[j]);
      else
	fprintf (fp, ".");
    for (; j < i+16; ++j)
      fprintf (fp, " ");
    fprintf (fp, "|\n");
  }
}

/**
 * Thread-safe strerror_r.
 *
 * This is a wrapper around the two variants of L<strerror_r(3)>
 * in glibc since it is hard to use correctly (RHBZ#2030396).
 *
 * The buffer passed in should be large enough to store the
 * error message (256 chars at least) and should be non-static.
 * Note that the buffer might not be used, use the return value.
 */
const char *
guestfs_int_strerror (int errnum, char *buf, size_t buflen)
{
#ifdef HAVE_DECL_STRERROR_R
#ifdef STRERROR_R_CHAR_P
  /* GNU strerror_r */
  return strerror_r (errnum, buf, buflen);
#else
  /* XSI-compliant strerror_r */
  int err = strerror_r (errnum, buf, buflen);
  if (err > 0)
    snprintf (buf, buflen, "error number %d", errnum);
  return buf;
#endif
#else /* !HAVE_DECL_STRERROR_R */
  return strerror (errnum);	/* YOLO it. */
#endif
}
