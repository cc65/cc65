#! /usr/bin/make -f
# -*- make -*-
#
# Main gcc Makefile.
# This makefile is maintained by Greg King <greg.king4@verizon.net>.

# Goals that are supported by the cc65 package
.PHONY:	all bins libs docs samples tests clean zap
.PHONY:	uninstall install install-bins install-libs install-docs install-samps

# If SYS is defined on this makefile's command-line, then we want it to go
# to "samples" and "tests", but not to the other rules.  So, we disable a
# feature of GNU make that would have given ${SYS} to every sub-make.
#MAKEOVERRIDES=
# (That trick has been disabled.)

# To compile with custom make-options, set them here; for example:
#MAKEOPTS = -j 2 CFLAGS=-O4 CC=clang
MAKEOPTS =

# The install prefix and directories
prefix		= /usr/local
exec_prefix	= $(prefix)

bindir		= $(exec_prefix)/bin
datadir		= $(prefix)/share
docdir		= $(datadir)/doc
libdir		= $(exec_prefix)/lib

CC65_DOC	= $(docdir)/cc65
CC65_HOME	= $(libdir)/cc65

CA65_INC	= $(CC65_HOME)/asminc
CC65_INC	= $(CC65_HOME)/include
LD65_CFG	= $(CC65_HOME)/cfg
LD65_LIB	= $(CC65_HOME)/lib
LD65_OBJ	= $(CC65_HOME)/obj

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
	@$(MAKE) -C src -f make/gcc.mak $(MAKEOPTS) \
	  CA65_INC=\\\"${CA65_INC}/\\\" CC65_INC=\\\"${CC65_INC}/\\\" \
	  LD65_CFG=\\\"${LD65_CFG}/\\\" LD65_LIB=\\\"${LD65_LIB}/\\\" \
	  LD65_OBJ=\\\"${LD65_OBJ}/\\\"

libs:
	@$(MAKE) -C libsrc

# This rule won't try to generate HTML files
# if a host system doesn't have LinuxDoc Tools.
docs:
	@if linuxdoc -B check doc/index >/dev/null 2>&1; \
	  then $(MAKE) -C doc $(MAKEOPTS) html; \
	  else echo '"LinuxDoc Tools" is not installed; skipping HTML documentation.'; \
	  fi

# Some platforms cannot compile all of the sample and library-test programs.
# So, these rules ignore errors.

samples:
	-@$(MAKE) -k -C samples prefix=$(prefix) $(SYS:%=SYS=%)

tests:
	-@$(MAKE) -k -C testcode/lib prefix=$(prefix) $(SYS:%=SYS=%)

clean zap:
	@$(MAKE) -C src -f make/gcc.mak $@
	@$(MAKE) -C libsrc $@
	@$(MAKE) -C doc $@
	@$(MAKE) -C samples $@
#	@$(MAKE) -C testcode/lib $@ $(SYS:%=SYS=%)

uninstall:	install-test
	cd $(bindir) && $(RM) ar65${EXT} ca65${EXT} cc65${EXT} cl65${EXT} \
	  co65${EXT} da65${EXT} ld65${EXT} od65${EXT} grc${EXT} ca65html
	$(RM) -R $(CC65_HOME) $(CC65_DOC)

install:	install-test install-dirs install-bins install-libs install-docs
	@echo
	@echo 'If you put the files into non-standard directories, then'
	@echo 'you might need to export some shell environment variables:'
	@echo
	@echo 'CC65_HOME=$(CC65_HOME)'
	@echo ' or'
	@echo 'CA65_INC=$(CA65_INC)'
	@echo 'CC65_INC=$(CC65_INC)'
	@echo 'LD65_CFG=$(LD65_CFG)'
	@echo 'LD65_LIB=$(LD65_LIB)'
	@echo 'LD65_OBJ=$(LD65_OBJ)'
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
install-dirs:	$(bindir) $(datadir) $(docdir) $(libdir) \
		$(CC65_DOC) $(CC65_HOME) \
		$(CA65_INC) $(CC65_INC) \
		$(CC65_INC)/em $(CC65_INC)/geos $(CC65_INC)/joystick \
		$(CC65_INC)/mouse $(CC65_INC)/sys $(CC65_INC)/tgi \
		$(LD65_CFG) $(LD65_LIB) $(LD65_OBJ) \
		$(CC65_HOME)/emd $(CC65_HOME)/joy $(CC65_HOME)/mou \
		$(CC65_HOME)/ser $(CC65_HOME)/tgi

$(bindir) $(datadir) $(docdir) $(libdir) \
$(CC65_DOC) $(CC65_HOME) \
$(CA65_INC) $(CC65_INC) \
$(LD65_CFG) $(LD65_LIB) $(LD65_OBJ):
	$(MKDIR) $@

$(CC65_HOME)/% $(CC65_INC)/% $(CC65_DOC)/%:
	$(MKDIR) $@

install-bins:
	for f in ar65 ca65 cc65 cl65 co65 da65 ld65 od65 grc; \
	  do $(INSTALL_STRIP) src/$$f/$$f${EXT} $(bindir) || exit $$?; \
	  done
	$(INSTALL_PROG) src/ca65html/ca65html $(bindir)

install-libs:
	for f in asminc/*.inc; \
	  do $(INSTALL_DATA) $$f $(CA65_INC) || exit $$?; \
	  done
	for f in include/*.h; \
	  do $(INSTALL_DATA) $$f $(CC65_INC) || exit $$?; \
	  done
	for d in em geos joystick mouse sys tgi; \
	  do for f in include/$$d/*.h; \
	    do $(INSTALL_DATA) $$f $(CC65_INC)/$$d || exit $$?; \
	    done || exit $$?; \
	  done
	for f in libsrc/*.lib; \
	  do $(INSTALL_DATA) $$f $(LD65_LIB) || exit $$?; \
	  done
	for f in libsrc/*-*.o; \
	  do $(INSTALL_DATA) $$f $(LD65_OBJ) || exit $$?; \
	  done
	for d in emd joy mou ser tgi; \
	  do for f in libsrc/*.$$d; \
	    do $(INSTALL_DATA) $$f $(CC65_HOME)/$$d || exit $$?; \
	    done || exit $$?; \
	  done
	for f in src/ld65/cfg/*-*.cfg; \
	  do $(INSTALL_DATA) $$f $(LD65_CFG) || exit $$?; \
	  done

install-docs:
	for f in src/ca65/macpack/*.mac; \
	  do $(INSTALL_DATA) $$f $(CC65_DOC) || exit $$?; \
	  done
	for f in readme.1st compile.txt CREDITS BUGS internal.txt newvers.txt; \
	  do $(INSTALL_DATA) doc/$$f $(CC65_DOC) || exit $$?; \
	  done
	if [ -f doc/index.htm* ]; \
	  then for f in doc/*.htm*; \
	    do $(INSTALL_DATA) $$f $(CC65_DOC) || exit $$?; \
	    done; \
	  fi

install-samps:	${addprefix $(CC65_DOC)/, $(shell find samples -type d)}
	@$(MAKE) -C samples zap
	for d in `find samples -type d`; \
	  do for f in $$d/*; \
	    do if [ -f $$f ]; \
	      then $(INSTALL_DATA) $$f $(CC65_DOC)/$$d || exit $$?; \
	      fi; \
	    done || exit $$?; \
	  done
