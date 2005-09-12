#! /usr/bin/make -f
# -*- makefile -*-
#
# Main gcc Makefile.
# This makefile is maintained by Greg King <gngking@erols.com>.

# Goals that are supported by the cc65 package
.PHONY:	all bins libs docs samples tests clean zap
.PHONY:	uninstall install install-bins install-libs install-docs

# If SYS is defined on this makefile's command-line, then we want it to go
# to "samples" and "tests", but not to the other rules.  So, we disable a
# feature of GNU make that would have given ${SYS} to every sub-make.
MAKEOVERRIDES=

# The install prefix and directories
prefix		= /usr
exec_prefix	= $(prefix)

bindir		= $(exec_prefix)/bin
datadir		= $(prefix)/share
docdir		= $(datadir)/doc
libdir		= $(exec_prefix)/lib

CC65DATA	= $(libdir)/cc65
CC65DOC		= $(docdir)/cc65

CC65ASM		= $(CC65DATA)/asminc
CC65INC		= $(CC65DATA)/include
CC65LIB		= $(CC65DATA)/lib

# Programs

MKDIR		= mkdir -m 755

# BSD-like install-program/-script
INSTALL		= install
#INSTALL		= install-sh

INSTALL_DATA	= $(INSTALL) -c -m 644
INSTALL_PROG	= $(INSTALL) -c -m 755
INSTALL_STRIP	= $(INSTALL_PROG) -s

# Rules

# The sample and library-test programs must be compiled for only one platform
# at a time.  So, those rules are done automatically only when a user names
# a system on the command-line.  (A user can do those rules with their
# defaults by putting "all samples tests" on the command-line.)
#
all:	bins libs docs $(SYS:%=samples tests)

bins:
	@$(MAKE) -C src -f make/gcc.mak

libs:
	@$(MAKE) -C libsrc

# A host system might not have LinuxDoc Tools, so this rule ignores errors.
docs:
	-@which sgml2html > /dev/null && $(MAKE) -C doc html || echo 'SGML-Tools not installed, skipping docs'

# Some platforms cannot compile all of the sample and library-test programs.
# So, these rules ignore errors.

samples:
	-@$(MAKE) -k -C samples $(SYS:%=SYS=%)

tests:
	-@$(MAKE) -k -C testcode/lib $(SYS:%=SYS=%)

clean zap:
	$(MAKE) -C src -f make/gcc.mak $@
	$(MAKE) -C libsrc $@
	$(MAKE) -C doc $@
	$(MAKE) -C samples $@
	$(MAKE) -C testcode/lib $@ $(SYS:%=SYS=%)

uninstall:	install-test
	cd $(bindir) && $(RM) ar65 ca65 cc65 cl65 co65 da65 ld65 od65 grc ca65html
	$(RM) -R $(CC65DATA) $(CC65DOC)

install:	install-test install-dirs install-bins install-libs install-docs
	@echo
	@echo 'You should export some shell environment variables:'
	@echo
	@echo 'CC65_INC=$(CC65INC)'
	@echo 'CC65_LIB=$(CC65LIB)'
	@echo

.PHONY:	install-test
install-test:
#	@if [ `id -u` != 0 ]; then					\
#	  echo >&2;							\
#	  echo 'Do "make install" or "make uninstall" as root.' >&2;	\
#	  echo >&2;							\
#	  false;							\
#	  fi

.PHONY:	install-dirs
install-dirs:
	-$(MKDIR) -p $(bindir) $(docdir) $(libdir)
	-$(MKDIR) $(CC65DATA) $(CC65ASM) $(CC65DOC) $(CC65INC)
	-$(MKDIR) $(CC65INC)/em $(CC65INC)/geos $(CC65INC)/joystick \
	  $(CC65INC)/mouse $(CC65INC)/sys $(CC65INC)/tgi
	-$(MKDIR) $(CC65LIB) $(CC65DATA)/emd $(CC65DATA)/joy \
	  $(CC65DATA)/mou $(CC65DATA)/ser $(CC65DATA)/tgi

install-bins:
	for f in ar65 ca65 cc65 cl65 co65 da65 ld65 od65 grc; \
	  do $(INSTALL_STRIP) src/$$f/$$f $(bindir) || exit 1; \
	  done
	$(INSTALL_PROG) src/ca65html/ca65html $(bindir)

install-libs:
	for f in asminc/*.inc; \
	  do $(INSTALL_DATA) $$f $(CC65ASM) || exit 1; \
	  done
	for f in include/*.h; \
	  do $(INSTALL_DATA) $$f $(CC65INC) || exit 1; \
	  done
	for d in em geos joystick mouse sys tgi; \
	  do for f in include/$$d/*.h; \
	    do $(INSTALL_DATA) $$f $(CC65INC)/$$d || exit 1; \
	    done || exit 1; \
	  done
	for f in libsrc/*.lib libsrc/*.o; \
	  do $(INSTALL_DATA) $$f $(CC65LIB) || exit 1; \
	  done
	for d in emd joy mou ser tgi; \
	  do for f in libsrc/*.$$d; \
	    do $(INSTALL_DATA) $$f $(CC65DATA)/$$d || exit 1; \
	    done || exit 1; \
	  done

install-docs:
	for f in src/ld65/cfg/*.cfg; \
	  do $(INSTALL_DATA) $$f $(CC65DOC) || exit 1; \
	  done
	for f in readme.1st compile.txt CREDITS BUGS internal.txt newvers.txt; \
	  do $(INSTALL_DATA) doc/$$f $(CC65DOC) || exit 1; \
	  done
	-for f in doc/*.html; \
	  do $(INSTALL_DATA) $$f $(CC65DOC) || exit 1; \
	  done
