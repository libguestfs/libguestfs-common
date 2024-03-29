# libguestfs virt-df
# Copyright (C) 2010-2019 Red Hat Inc.
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

# libparallel.la contains a framework used for processing multiple
# libvirt domains in parallel.  It is used by virt-df and
# virt-alignment-scan.
noinst_LTLIBRARIES = libparallel.la

libparallel_la_SOURCES = \
	domains.c \
	domains.h \
	estimate-max-threads.c \
	estimate-max-threads.h \
	parallel.c \
	parallel.h
libparallel_la_CPPFLAGS = \
	-DGUESTFS_NO_DEPRECATED=1 \
	-I$(top_srcdir)/common/utils -I$(top_builddir)/common/utils \
	-I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib \
	-I$(top_srcdir)/lib -I$(top_builddir)/lib \
	$(INCLUDE_DIRECTORY) \
	-I$(top_srcdir)/common/options -I$(top_builddir)/common/options
libparallel_la_CFLAGS = \
	-pthread \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	$(LIBGUESTFS_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(LIBVIRT_CFLAGS)
libparallel_la_LIBADD = \
	$(top_builddir)/common/options/liboptions.la \
	$(top_builddir)/common/utils/libutils.la \
	$(LIBXML2_LIBS) \
	$(LIBVIRT_LIBS) \
	$(LTLIBINTL) \
	$(top_builddir)/gnulib/lib/libgnu.la \
	-lm
