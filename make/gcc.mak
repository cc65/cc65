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
prefix		= /usr/local
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

# BSD-like install-script/-program
INSTALL		= make/install-sh

INSTALL_DATA	= $(INSTALL) -c -m 644
INSTALL_PROG	= $(INSTALL) -c -m 755
INSTALL_STRIP	= $(INSTALL_PROG) -s

# This file-name extension is needed on DOS/Windows systems.
ifdef COMSPEC
EXT	= .exe
endif

# Rules

# The sample and library-test programs must be compiled for only one platform
# at a time.  So, those rules are done automatically only when a user names
# a system on the command-line.  (A user can do those rules with their
# defaults by putting "all samples tests" on the command-line.)
#
all:	bins libs docs $(SYS:%=samples tests)

bins:
	@$(MAKE) -C src -f make/gcc.mak CC65_INC=\\\"${CC65INC}/\\\" CC65_LIB=\\\"${CC65LIB}/\\\"

libs:
	@$(MAKE) -C libsrc

# This rule won't try to generate HTML files
# if a host system doesn't have LinuxDoc Tools.
docs:
	@if sgmlcheck doc/index >/dev/null 2>&1; \
	  then $(MAKE) -C doc html; \
	  else echo '"LinuxDoc Tools" is not installed; skipping HTML documentation.'; \
	  fi

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
#	$(MAKE) -C testcode/lib $@ $(SYS:%=SYS=%)

uninstall:	install-test
	cd $(bindir) && $(RM) ar65${EXT} ca65${EXT} cc65${EXT} cl65${EXT} \
	  co65${EXT} da65${EXT} ld65${EXT} od65${EXT} grc${EXT} ca65html
	$(RM) -R $(CC65DATA) $(CC65DOC)

install:	install-test install-dirs install-bins install-libs install-docs
	@echo
	@echo 'You can export some shell environment variables:'
	@echo
	@echo 'CC65_INC=$(CC65INC)'
	@echo 'CC65_LIB=$(CC65LIB)'
	@echo

.PHONY:	install-test
install-test:
	@if [ `id -u` != 0 ]; then \
	  echo; \
	  echo 'If you are denied permission to install or uninstall this package,'; \
	  echo 'then you will need to do "make/gcc.mak install" or "make/gcc.mak uninstall"'; \
	  echo 'as either the root user or an administrator.'; \
	  echo; \
	  fi 2>/dev/null

.PHONY:	install-dirs
install-dirs:
	[ -d $(bindir) ] || $(MKDIR) $(bindir)
	[ -d $(datadir) ] || $(MKDIR) $(datadir)
	[ -d $(docdir) ] || $(MKDIR) $(docdir)
	[ -d $(libdir) ] || $(MKDIR) $(libdir)
	[ -d $(CC65DOC) ] || $(MKDIR) $(CC65DOC)
	[ -d $(CC65DATA) ] || $(MKDIR) $(CC65DATA)
	[ -d $(CC65ASM) ] || $(MKDIR) $(CC65ASM)
	[ -d $(CC65LIB) ] || $(MKDIR) $(CC65LIB)
	[ -d $(CC65INC) ] || $(MKDIR) $(CC65INC)
	[ -d $(CC65INC)/em ] || $(MKDIR) $(CC65INC)/em
	[ -d $(CC65INC)/geos ] || $(MKDIR) $(CC65INC)/geos
	[ -d $(CC65INC)/joystick ] || $(MKDIR) $(CC65INC)/joystick
	[ -d $(CC65INC)/mouse ] || $(MKDIR) $(CC65INC)/mouse
	[ -d $(CC65INC)/sys ] || $(MKDIR) $(CC65INC)/sys
	[ -d $(CC65INC)/tgi ] || $(MKDIR) $(CC65INC)/tgi
	[ -d $(CC65DATA)/emd ] || $(MKDIR) $(CC65DATA)/emd
	[ -d $(CC65DATA)/joy ] || $(MKDIR) $(CC65DATA)/joy
	[ -d $(CC65DATA)/mou ] || $(MKDIR) $(CC65DATA)/mou
	[ -d $(CC65DATA)/ser ] || $(MKDIR) $(CC65DATA)/ser
	[ -d $(CC65DATA)/tgi ] || $(MKDIR) $(CC65DATA)/tgi

install-bins:
	for f in ar65 ca65 cc65 cl65 co65 da65 ld65 od65 grc; \
	  do $(INSTALL_STRIP) src/$$f/$$f${EXT} $(bindir) || exit 1; \
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
	for f in src/ld65/cfg/*.cfg src/ca65/macpack/*.mac; \
	  do $(INSTALL_DATA) $$f $(CC65DOC) || exit 1; \
	  done
	for f in readme.1st compile.txt CREDITS BUGS internal.txt newvers.txt; \
	  do $(INSTALL_DATA) doc/$$f $(CC65DOC) || exit 1; \
	  done
	if [ -e doc/index.htm* ]; \
	  then for f in doc/*.htm*; \
	    do $(INSTALL_DATA) $$f $(CC65DOC) || exit 1; \
	    done; \
	  fi
