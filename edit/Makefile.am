# libguestfs
# Copyright (C) 2009-2019 Red Hat Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

include $(top_srcdir)/subdir-rules.mk

# libedit.la contains the common code used by virt-edit, guestfish
# 'edit' command, virt-customize and virt-builder.
noinst_LTLIBRARIES = libedit.la

libedit_la_SOURCES = \
	file-edit.c \
	file-edit.h
libedit_la_CPPFLAGS = \
	-DGUESTFS_NO_DEPRECATED=1 \
	-I$(top_srcdir)/common/utils -I$(top_builddir)/common/utils \
	-I$(top_srcdir)/lib -I$(top_builddir)/lib \
	$(INCLUDE_DIRECTORY)
libedit_la_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	$(LIBGUESTFS_CFLAGS)
libedit_la_LIBADD = \
	$(top_builddir)/common/utils/libutils.la
