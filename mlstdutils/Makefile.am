# libguestfs OCaml tools common code
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
	$(SOURCES_MLI) \
	$(filter-out $(CONFIGURE_GENERATED_ML),$(SOURCES_ML)) \
	std_utils_tests.ml

CONFIGURE_GENERATED_ML = \
	guestfs_config.ml

SOURCES_MLI = \
	guestfs_config.mli \
	std_utils.mli \
	stringMap.mli \
	stringSet.mli

SOURCES_ML = \
	$(CONFIGURE_GENERATED_ML) \
	stringMap.ml \
	stringSet.ml \
	std_utils.ml

# We pretend that we're building a C library.  automake handles the
# compilation of the C sources for us.  At the end we take the C
# objects and OCaml objects and link them into the OCaml library.
# This C library is never used.

noinst_LIBRARIES = libmlstdutils.a

if !HAVE_OCAMLOPT
MLSTDUTILS_CMA = mlstdutils.cma
else
MLSTDUTILS_CMA = mlstdutils.cmxa
endif

# Just for this library, we need to build both bytecode and native
# code because the generator always requires the bytecode version.
noinst_DATA = mlstdutils.cma
if HAVE_OCAMLOPT
noinst_DATA += mlstdutils.cmxa
endif

libmlstdutils_a_SOURCES = dummy.c
libmlstdutils_a_CPPFLAGS = \
	-I. \
	-I$(top_builddir)
libmlstdutils_a_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	-fPIC

BOBJECTS = $(SOURCES_ML:.ml=.cmo)
XOBJECTS = $(BOBJECTS:.cmo=.cmx)

OCAMLPACKAGES = \
	-package str,unix \
	-I $(builddir)
OCAMLPACKAGES_TESTS = $(MLSTDUTILS_CMA)

OCAMLFLAGS = $(OCAML_FLAGS) $(OCAML_WARN_ERROR) -ccopt '$(CFLAGS)'

if !HAVE_OCAMLOPT
OBJECTS = $(BOBJECTS)
else
OBJECTS = $(XOBJECTS)
endif

libmlstdutils_a_DEPENDENCIES = $(OBJECTS)

mlstdutils.cma: $(BOBJECTS)
	$(OCAMLFIND) ocamlc $(OCAMLPACKAGES) -a $^ -o $@

if HAVE_OCAMLOPT
mlstdutils.cmxa: $(XOBJECTS)
	$(OCAMLFIND) ocamlopt $(OCAMLPACKAGES) -a $^ -o $@
endif

# Tests.

std_utils_tests_SOURCES = dummy.c
std_utils_tests_CPPFLAGS = \
	-I. \
	-I$(top_builddir)
std_utils_tests_BOBJECTS = std_utils_tests.cmo
std_utils_tests_XOBJECTS = $(std_utils_tests_BOBJECTS:.cmo=.cmx)

# Can't call the following as <test>_OBJECTS because automake gets confused.
if !HAVE_OCAMLOPT
std_utils_tests_THEOBJECTS = $(std_utils_tests_BOBJECTS)
std_utils_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)
else
std_utils_tests_THEOBJECTS = $(std_utils_tests_XOBJECTS)
std_utils_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)
endif

OCAMLLINKFLAGS = $(LINK_CUSTOM_OCAMLC_ONLY)

std_utils_tests_DEPENDENCIES = \
	$(std_utils_tests_THEOBJECTS) \
	$(MLSTDUTILS_CMA) \
	$(top_srcdir)/ocaml-link.sh
std_utils_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(std_utils_tests_THEOBJECTS) -o $@

TESTS_ENVIRONMENT = $(top_builddir)/run --test

check_PROGRAMS = std_utils_tests
TESTS = std_utils_tests

check-valgrind:
	$(MAKE) VG="@VG@" check

# OCaml dependencies.
.depend: $(srcdir)/*.mli $(srcdir)/*.ml
	$(top_builddir)/ocaml-dep.sh $^
-include .depend

.PHONY: docs
