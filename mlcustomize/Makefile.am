# libguestfs OCaml virt-customize common code
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
	$(SOURCES_C) \
	test-firstboot.sh \
	test-selinuxrelabel.sh

generator_built = \
	customize_cmdline.mli \
	customize_cmdline.ml \
	customize-options.pod \
	customize-synopsis.pod \
	v2v-customize-options.pod \
	v2v-customize-synopsis.pod

SOURCES_MLI = \
	append_line.mli \
	crypt.mli \
	customize_cmdline.mli \
	customize_run.mli \
	firstboot.mli \
	guest_packages.mli \
	hostname.mli \
	inject_virtio_win.mli \
	password.mli \
	perl_edit.mli \
	random_seed.mli \
	SELinux_relabel.mli \
	ssh_key.mli \
	subscription_manager.mli \
	timezone.mli

# This list must be in dependency order.
SOURCES_ML = \
	firstboot.ml \
	append_line.ml \
	hostname.ml \
	perl_edit.ml \
	random_seed.ml \
	ssh_key.ml \
	subscription_manager.ml \
	timezone.ml \
	crypt.ml \
	password.ml \
	guest_packages.ml \
	inject_virtio_win.ml \
	SELinux_relabel.ml \
	customize_cmdline.ml \
	customize_run.ml

SOURCES_C = \
	../edit/file-edit.c \
	../edit/file-edit.h \
	crypt-c.c \
	perl_edit-c.c

if HAVE_OCAML

# We pretend that we're building a C library.  automake handles the
# compilation of the C sources for us.  At the end we take the C
# objects and OCaml objects and link them into the OCaml library.
# This C library is never used.

noinst_LIBRARIES = libmlcustomize.a

if !HAVE_OCAMLOPT
MLCUSTOMIZE_CMA = mlcustomize.cma
else
MLCUSTOMIZE_CMA = mlcustomize.cmxa
endif

noinst_DATA = $(MLCUSTOMIZE_CMA)

libmlcustomize_a_SOURCES = $(SOURCES_C)
libmlcustomize_a_CPPFLAGS = \
	-DCAML_NAME_SPACE \
	-I. \
	-I$(top_builddir) \
	-I$(top_srcdir)/gnulib/lib -I$(top_builddir)/gnulib/lib \
	-I$(shell $(OCAMLC) -where) \
	-I$(top_srcdir)/common/utils \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/common/edit \
	-I$(top_srcdir)/common/mlstdutils \
	-I$(top_srcdir)/common/mlgettext \
	-I$(top_srcdir)/common/mlpcre \
	-I$(top_srcdir)/common/mltools \
	-I$(top_srcdir)/common/mlxml
libmlcustomize_a_CFLAGS = \
	$(WARN_CFLAGS) $(WERROR_CFLAGS) \
	-fPIC

BOBJECTS = $(SOURCES_ML:.ml=.cmo)
XOBJECTS = $(BOBJECTS:.cmo=.cmx)

OCAMLPACKAGES = \
	-package str,unix,guestfs \
	-I $(top_builddir)/common/utils/.libs \
	-I $(top_builddir)/ocaml \
	-I $(top_builddir)/common/mlstdutils \
	-I $(top_builddir)/common/mlgettext \
	-I $(top_builddir)/common/mlpcre \
	-I $(top_builddir)/common/mltools \
	-I $(top_builddir)/common/mlxml \
	-I $(builddir)
OCAMLPACKAGES_TESTS = $(MLCUSTOMIZE_CMA)
if HAVE_OCAML_PKG_GETTEXT
OCAMLPACKAGES += -package gettext-stub
endif

OCAMLCLIBS = \
	-lutils \
	$(LIBINTL) \
	-lgnu

OCAMLFLAGS = $(OCAML_FLAGS) $(OCAML_WARN_ERROR) -ccopt '$(CFLAGS)'

if !HAVE_OCAMLOPT
OBJECTS = $(BOBJECTS)
else
OBJECTS = $(XOBJECTS)
endif

libmlcustomize_a_DEPENDENCIES = $(OBJECTS)

$(MLCUSTOMIZE_CMA): $(OBJECTS) libmlcustomize.a
	$(OCAMLFIND) mklib $(OCAMLPACKAGES) \
	    $(OBJECTS) $(libmlcustomize_a_OBJECTS) -o mlcustomize

# Tests.

TESTS_ENVIRONMENT = $(top_builddir)/run --test

TESTS = $(SLOW_TESTS)

check-valgrind:
	$(MAKE) VG="@VG@" check

# Slow tests of virt-customize functionality in real guests.

SLOW_TESTS = \
	$(firstboot_test_scripts) \
	test-selinuxrelabel.sh

check-slow:
	$(MAKE) check TESTS="$(SLOW_TESTS)" SLOW=1

firstboot_test_scripts := \
	test-firstboot-rhel-4.9.sh \
	test-firstboot-rhel-5.11.sh \
	test-firstboot-rhel-6.8.sh \
	test-firstboot-rhel-7.2.sh \
	test-firstboot-debian-6.sh \
	test-firstboot-debian-7.sh \
	test-firstboot-debian-8.sh \
	test-firstboot-fedora-26.sh \
	test-firstboot-fedora-27.sh \
	test-firstboot-ubuntu-10.04.sh \
	test-firstboot-ubuntu-12.04.sh \
	test-firstboot-ubuntu-14.04.sh \
	test-firstboot-ubuntu-16.04.sh \
	test-firstboot-ubuntu-18.04.sh \
	test-firstboot-windows-6.2-server.sh \
	test-firstboot-windows-6.3-server.sh \
	test-firstboot-windows-10.0-server.sh
# Firstboot is known-broken on RHEL 3:
#	test-firstboot-rhel-3.9.sh

test-firstboot-%.sh:
	rm -f $@ $@-t
	f=`echo "$@" | $(SED) 's/test-firstboot-\(.*\).sh/\1/'`; \
	echo 'script=$@ exec $$srcdir/test-firstboot.sh' "$$f" > $@-t
	chmod 0755 $@-t
	mv $@-t $@

CLEANFILES += \
	$(firstboot_test_scripts) \
	firstboot-*.img

# Dependencies.
.depend: $(srcdir)/*.mli $(srcdir)/*.ml
	$(top_builddir)/ocaml-dep.sh $^
-include .depend

endif

.PHONY: docs
