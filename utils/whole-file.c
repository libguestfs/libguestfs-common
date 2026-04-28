/* libguestfs
 * Copyright (C) 2011-2026 Red Hat Inc.
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libintl.h>

#include "guestfs-utils.h"

/**
 * Read the whole file C<filename> into a memory buffer.
 *
 * The memory buffer is initialized and returned in C<data_r>.  The
 * size of the file in bytes is returned in C<size_r>.  The return
 * buffer must be freed by the caller.
 *
 * On error this prints an error on C<stderr> and returns -1.  Unlike
 * the similar C<guestfs_int_read_whole_file> this does not use the
 * libguestfs handle or call C<error()>.
 *
 * For the convenience of callers, the returned buffer is
 * NUL-terminated (the NUL is not included in the size).
 *
 * The file must be a B<regular>, B<local>, B<trusted> file.  In
 * particular, do not use this function to read files that might be
 * under control of an untrusted user since that will lead to a
 * denial-of-service attack.
 */
int
read_whole_file (const char *filename, char **data_r, size_t *size_r)
{
  int fd;
  char *data;
  off_t size;
  off_t n;
  ssize_t r;
  struct stat statbuf;

  fd = open (filename, O_RDONLY|O_CLOEXEC);
  if (fd == -1) {
    perror (filename);
    return -1;
  }

  if (fstat (fd, &statbuf) == -1) {
    perror (filename);
    close (fd);
    return -1;
  }

  size = statbuf.st_size;
  data = malloc (size + 1);
  if (data == NULL) {
    perror ("malloc");
    close (fd);
    return -1;
  }

  n = 0;
  while (n < size) {
    r = read (fd, &data[n], size - n);
    if (r == -1) {
      perror (filename);
      free (data);
      close (fd);
      return -1;
    }
    if (r == 0) {
      fprintf (stderr, "%s: unexpected end of input", filename);
      free (data);
      close (fd);
      return -1;
    }
    n += r;
  }

  if (close (fd) == -1) {
    perror (filename);
    free (data);
    return -1;
  }

  /* For convenience of callers, \0-terminate the data. */
  data[size] = '\0';

  *data_r = data;
  if (size_r != NULL)
    *size_r = size;

  return 0;
}
