# libguestfs OCaml virt-v2v generated code
# Copyright (C) 2011-2019 Red Hat Inc.
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
	$(generator_built) \
	$(SOURCES_MLI) \
	$(SOURCES_ML) \
	$(SOURCES_C)

generator_built = \
	uefi.mli \
	uefi.ml

SOURCES_MLI = \
	uefi.mli

SOURCES_ML = \
	uefi.ml

SOURCES_C = \
	dummy.c

# We pretend that we're building a C library.  automake handles the
# compilation of the C sources for us.  At the end we take the C
# objects and OCaml objects and link them into the OCaml library.
# This C library is never used.

noinst_LIBRARIES = libmlv2v.a

if !HAVE_OCAMLOPT
MLV2V_CMA = mlv2v.cma
else
MLV2V_CMA = mlv2v.cmxa
endif

noinst_DATA = $(MLV2V_CMA)

libmlv2v_a_SOURCES = $(SOURCES_C)
libmlv2v_a_CPPFLAGS = \
	-DCAML_NAME_SPACE \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where)
libmlv2v_a_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	-fPIC

BOBJECTS = $(SOURCES_ML:.ml=.cmo)
XOBJECTS = $(BOBJECTS:.cmo=.cmx)

OCAMLPACKAGES = \
	-package str,unix \
	-I $(builddir)
OCAMLPACKAGES_TESTS = $(MLV2V_CMA)

OCAMLFLAGS = $(OCAML_FLAGS) $(OCAML_WARN_ERROR) -ccopt '$(CFLAGS)'

if !HAVE_OCAMLOPT
OBJECTS = $(BOBJECTS)
else
OBJECTS = $(XOBJECTS)
endif

libmlv2v_a_DEPENDENCIES = $(OBJECTS)

$(MLV2V_CMA): $(OBJECTS) libmlv2v.a
	$(OCAMLFIND) mklib $(OCAMLPACKAGES) \
	    $(OBJECTS) $(libmlv2v_a_OBJECTS) -o mlv2v

# OCaml dependencies.
.depend: $(srcdir)/*.mli $(srcdir)/*.ml
	$(top_builddir)/ocaml-dep.sh $^
-include .depend

.PHONY: docs
