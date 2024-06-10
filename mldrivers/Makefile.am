# Common code for driver detection used by virt-drivers and virt-v2v
# Copyright (C) 2011-2023 Red Hat Inc.
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

EXTRA_DIST = \
	$(SOURCES_MLI) \
	$(SOURCES_ML) \
	$(SOURCES_C)

SOURCES_MLI = \
	firmware.mli \
	linux.mli \
	linux_bootloaders.mli \
	linux_kernels.mli \
	windows_drivers.mli

SOURCES_ML = \
	firmware.ml \
	linux.ml \
	linux_bootloaders.ml \
	linux_kernels.ml \
	windows_drivers.ml

SOURCES_C = \
	dummy.c

# We pretend that we're building a C library.  automake handles the
# compilation of the C sources for us.  At the end we take the C
# objects and OCaml objects and link them into the OCaml library.
# This C library is never used.

noinst_LIBRARIES = libmldrivers.a

if !HAVE_OCAMLOPT
MLDRIVERS_CMA = mldrivers.cma
else
MLDRIVERS_CMA = mldrivers.cmxa
endif

noinst_DATA = $(MLDRIVERS_CMA)

libmldrivers_a_SOURCES = $(SOURCES_C)
libmldrivers_a_CPPFLAGS = \
	-DCAML_NAME_SPACE \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/common/mlgettext \
	-I$(top_srcdir)/common/mlpcre \
	-I$(top_srcdir)/common/mlstdutils \
	-I$(top_srcdir)/common/mltools
libmldrivers_a_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	-fPIC

BOBJECTS = $(SOURCES_ML:.ml=.cmo)
XOBJECTS = $(BOBJECTS:.cmo=.cmx)

OCAMLPACKAGES = \
	-package str,unix,guestfs \
	-I $(top_builddir)/common/mlgettext \
	-I $(top_builddir)/common/mlpcre \
	-I $(top_builddir)/common/mlstdutils \
	-I $(top_builddir)/common/mltools \
	-I $(builddir)
OCAMLPACKAGES_TESTS = $(MLDRIVERS_CMA)

OCAMLFLAGS = $(OCAML_FLAGS) $(OCAML_WARN_ERROR) -ccopt '$(CFLAGS)'

if !HAVE_OCAMLOPT
OBJECTS = $(BOBJECTS)
else
OBJECTS = $(XOBJECTS)
endif

libmldrivers_a_DEPENDENCIES = $(OBJECTS)

$(MLDRIVERS_CMA): $(OBJECTS) libmldrivers.a
	$(OCAMLFIND) mklib $(OCAMLPACKAGES) \
	    $(OBJECTS) $(libmldrivers_a_OBJECTS) -o mldrivers

# OCaml dependencies.
.depend: $(srcdir)/*.mli $(srcdir)/*.ml
	$(top_builddir)/ocaml-dep.sh $^
-include .depend

.PHONY: docs
