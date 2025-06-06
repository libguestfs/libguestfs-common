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
	$(SOURCES_ML) \
	$(SOURCES_C) \
	getopt_tests.ml \
	JSON_tests.ml \
	JSON_parser_tests.ml \
	machine_readable_tests.ml \
	parse_tools_messages_test.py \
	test-getopt.sh \
	test-machine-readable.sh \
	test-tools-messages.sh \
	tools_messages_tests.ml \
	tools_utils_tests.ml

SOURCES_MLI = \
	checksums.mli \
	curl.mli \
	DOM.mli \
	getopt.mli \
	JSON.mli \
	JSON_parser.mli \
	libosinfo.mli \
	libosinfo_utils.mli \
	on_exit.mli \
	planner.mli \
	regedit.mli \
	registry.mli \
	tools_utils.mli \
	urandom.mli \
	URI.mli \
	xpath_helpers.mli

SOURCES_ML = \
	getopt.ml \
	JSON.ml \
	tools_utils.ml \
	DOM.ml \
	on_exit.ml \
	URI.ml \
	planner.ml \
	registry.ml \
	regedit.ml \
	JSON_parser.ml \
	curl.ml \
	checksums.ml \
	xpath_helpers.ml \
	urandom.ml \
	libosinfo.ml \
	libosinfo_utils.ml

SOURCES_C = \
	../options/decrypt.c \
	../options/keys.c \
	../options/uri.c \
	getopt-c.c \
	JSON_parser-c.c \
	libosinfo-c.c \
	tools_utils-c.c \
	uri-c.c

if HAVE_OCAML

# We pretend that we're building a C library.  automake handles the
# compilation of the C sources for us.  At the end we take the C
# objects and OCaml objects and link them into the OCaml library.
# This C library is never used.

noinst_LIBRARIES = libmltools.a

if !HAVE_OCAMLOPT
MLTOOLS_CMA = mltools.cma
else
MLTOOLS_CMA = mltools.cmxa
endif

noinst_DATA = $(MLTOOLS_CMA)

libmltools_a_SOURCES = $(SOURCES_C)
libmltools_a_CPPFLAGS = \
	-DCAML_NAME_SPACE \
	-I. \
	-I$(top_builddir) \
	-I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/common/utils \
	-I$(top_srcdir)/lib \
	$(INCLUDE_DIRECTORY) \
	-I$(top_srcdir)/common/options \
	-I$(top_srcdir)/common/mlgettext \
	-I$(top_srcdir)/common/mlpcre \
	-I$(top_srcdir)/common/mlxml \
	-I$(top_srcdir)/common/mlstdutils \
	-I$(top_srcdir)/common/mlutils
libmltools_a_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	$(LIBVIRT_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(LIBGUESTFS_CFLAGS) \
	$(JSON_C_CFLAGS) \
	$(LIBOSINFO_CFLAGS) \
	-fPIC

BOBJECTS = $(SOURCES_ML:.ml=.cmo)
XOBJECTS = $(BOBJECTS:.cmo=.cmx)

OCAMLPACKAGES = \
	-package str,unix,guestfs \
	-I $(top_builddir)/common/utils/.libs \
	-I $(top_builddir)/gnulib/lib/.libs \
	-I $(top_builddir)/ocaml \
	-I $(top_builddir)/common/mlgettext \
	-I $(top_builddir)/common/mlpcre \
	-I $(top_builddir)/common/mlxml \
	-I $(top_builddir)/common/mlstdutils \
	-I $(top_builddir)/common/mlutils \
	-I $(builddir)
OCAMLPACKAGES_TESTS = $(MLTOOLS_CMA)
if HAVE_OCAML_PKG_GETTEXT
OCAMLPACKAGES += -package gettext-stub
endif

OCAMLCLIBS = \
	-lutils \
	$(LIBTINFO_LIBS) \
	$(LIBCRYPT_LIBS) \
	$(LIBVIRT_LIBS) \
	$(LIBXML2_LIBS) \
	$(JSON_C_LIBS) \
	$(LIBOSINFO_LIBS) \
	$(LIBINTL) \
	-lgnu

OCAMLFLAGS = $(OCAML_FLAGS) $(OCAML_WARN_ERROR) -ccopt '$(CFLAGS)'

if !HAVE_OCAMLOPT
OBJECTS = $(BOBJECTS)
else
OBJECTS = $(XOBJECTS)
endif

libmltools_a_DEPENDENCIES = $(OBJECTS)

$(MLTOOLS_CMA): $(OBJECTS) libmltools.a
	$(OCAMLFIND) mklib $(OCAMLPACKAGES) \
	    $(OBJECTS) $(libmltools_a_OBJECTS) -o mltools

# Tests.

tools_utils_tests_SOURCES = dummy.c
tools_utils_tests_CPPFLAGS = \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/lib
tools_utils_tests_BOBJECTS = tools_utils_tests.cmo
tools_utils_tests_XOBJECTS = $(tools_utils_tests_BOBJECTS:.cmo=.cmx)

getopt_tests_SOURCES = dummy.c
getopt_tests_CPPFLAGS = \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/lib
getopt_tests_BOBJECTS = getopt_tests.cmo
getopt_tests_XOBJECTS = $(getopt_tests_BOBJECTS:.cmo=.cmx)

JSON_tests_SOURCES = dummy.c
JSON_tests_BOBJECTS = JSON_tests.cmo
JSON_tests_XOBJECTS = $(JSON_tests_BOBJECTS:.cmo=.cmx)

JSON_parser_tests_SOURCES = dummy.c
JSON_parser_tests_CPPFLAGS = \
	-I . \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/lib
JSON_parser_tests_BOBJECTS = \
	JSON_parser_tests.cmo
JSON_parser_tests_XOBJECTS = $(JSON_parser_tests_BOBJECTS:.cmo=.cmx)

machine_readable_tests_SOURCES = dummy.c
machine_readable_tests_CPPFLAGS = \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/lib
machine_readable_tests_BOBJECTS = machine_readable_tests.cmo
machine_readable_tests_XOBJECTS = $(machine_readable_tests_BOBJECTS:.cmo=.cmx)

tools_messages_tests_SOURCES = dummy.c
tools_messages_tests_CPPFLAGS = \
	-I. \
	-I$(top_builddir) \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/lib
tools_messages_tests_BOBJECTS = tools_messages_tests.cmo
tools_messages_tests_XOBJECTS = $(tools_messages_tests_BOBJECTS:.cmo=.cmx)

# Can't call the following as <test>_OBJECTS because automake gets confused.
if !HAVE_OCAMLOPT
tools_utils_tests_THEOBJECTS = $(tools_utils_tests_BOBJECTS)
tools_utils_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

getopt_tests_THEOBJECTS = $(getopt_tests_BOBJECTS)
getopt_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

JSON_tests_THEOBJECTS = $(JSON_tests_BOBJECTS)
JSON_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

JSON_parser_tests_THEOBJECTS = $(JSON_parser_tests_BOBJECTS)
JSON_parser_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

machine_readable_tests_THEOBJECTS = $(machine_readable_tests_BOBJECTS)
machine_readable_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

tools_messages_tests_THEOBJECTS = $(tools_messages_tests_tests_BOBJECTS)
tools_messages_tests.cmo: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)
else
tools_utils_tests_THEOBJECTS = $(tools_utils_tests_XOBJECTS)
tools_utils_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

getopt_tests_THEOBJECTS = $(getopt_tests_XOBJECTS)
getopt_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

JSON_tests_THEOBJECTS = $(JSON_tests_XOBJECTS)
JSON_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

JSON_parser_tests_THEOBJECTS = $(JSON_parser_tests_XOBJECTS)
JSON_parser_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

machine_readable_tests_THEOBJECTS = $(machine_readable_tests_XOBJECTS)
machine_readable_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)

tools_messages_tests_THEOBJECTS = $(tools_messages_tests_XOBJECTS)
tools_messages_tests.cmx: OCAMLPACKAGES += $(OCAMLPACKAGES_TESTS)
endif

OCAMLLINKFLAGS = \
	mlstdutils.$(MLARCHIVE) \
	mlcutils.$(MLARCHIVE) \
	mlgettext.$(MLARCHIVE) \
	mlpcre.$(MLARCHIVE) \
	mlguestfs.$(MLARCHIVE) \
	$(LINK_CUSTOM_OCAMLC_ONLY)

tools_utils_tests_DEPENDENCIES = \
	$(tools_utils_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
tools_utils_tests_OCAMLCLIBS = \
	-pthread -lpthread \
	-lutils \
	$(LIBXML2_LIBS) \
	-lgnu \
	$(LIBGUESTFS_LIBS)
tools_utils_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '$(tools_utils_tests_OCAMLCLIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(tools_utils_tests_THEOBJECTS) -o $@

getopt_tests_DEPENDENCIES = \
	$(getopt_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
getopt_tests_OCAMLCLIBS = \
	-pthread -lpthread \
	-lutils \
	$(LIBXML2_LIBS) \
	-lgnu \
	$(LIBGUESTFS_LIBS)
getopt_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '$(getopt_tests_OCAMLCLIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(getopt_tests_THEOBJECTS) -o $@

JSON_tests_DEPENDENCIES = \
	$(JSON_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
JSON_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '-pthread -lpthread $(LIBGUESTFS_LIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(JSON_tests_THEOBJECTS) -o $@

JSON_parser_tests_DEPENDENCIES = \
	$(JSON_parser_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
JSON_parser_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '-pthread -lpthread $(OCAMLCLIBS) $(LIBGUESTFS_LIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(JSON_parser_tests_THEOBJECTS) -o $@

machine_readable_tests_DEPENDENCIES = \
	$(machine_readable_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
machine_readable_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '-pthread -lpthread -lutils -lgnu $(LIBGUESTFS_LIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(machine_readable_tests_THEOBJECTS) -o $@

tools_messages_tests_DEPENDENCIES = \
	$(tools_messages_tests_THEOBJECTS) \
	../mlstdutils/mlstdutils.$(MLARCHIVE) \
	../mlgettext/mlgettext.$(MLARCHIVE) \
	../mlpcre/mlpcre.$(MLARCHIVE) \
	$(MLTOOLS_CMA) \
	$(top_srcdir)/ocaml-link.sh
tools_messages_tests_LINK = \
	$(top_srcdir)/ocaml-link.sh \
	  -cclib '-pthread -lpthread -lutils -lgnu $(LIBGUESTFS_LIBS)' -- \
	  $(OCAMLFIND) $(BEST) $(OCAMLFLAGS) $(OCAMLLINKFLAGS) \
	  $(OCAMLPACKAGES) $(OCAMLPACKAGES_TESTS) \
	  $(tools_messages_tests_THEOBJECTS) -o $@

TESTS_ENVIRONMENT = $(top_builddir)/run --test

TESTS = \
	test-getopt.sh \
	test-machine-readable.sh \
	JSON_tests \
	JSON_parser_tests \
	tools_utils_tests
if HAVE_PYTHON
TESTS += \
	test-tools-messages.sh
endif
check_PROGRAMS = \
	getopt_tests \
	machine_readable_tests \
	tools_messages_tests \
	JSON_tests \
	JSON_parser_tests \
	tools_utils_tests

check-valgrind:
	$(MAKE) VG="@VG@" check

# Dependencies.
.depend: $(srcdir)/*.mli $(srcdir)/*.ml
	$(top_builddir)/ocaml-dep.sh $^
-include .depend

endif

.PHONY: docs
