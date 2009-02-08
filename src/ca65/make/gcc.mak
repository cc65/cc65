#
# gcc Makefile for ca65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= ca65

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -W -std=c89 -I$(COMMON)
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

# Perl script for macro file conversion
CVT=macpack/cvt-mac.pl

# -----------------------------------------------------------------------------
# List of all object files

OBJS =  anonname.o      \
        asserts.o       \
        condasm.o	\
	dbginfo.o	\
       	ea65.o	      	\
        easw16.o        \
        enum.o          \
        error.o	      	\
        expr.o	      	\
	feature.o	\
	filetab.o	\
	fragment.o	\
        global.o       	\
	incpath.o      	\
        instr.o	       	\
	istack.o       	\
	lineinfo.o	\
	listing.o      	\
	macpack.o      	\
       	macro.o	       	\
        main.o 	       	\
	nexttok.o      	\
        objcode.o      	\
        objfile.o      	\
       	options.o      	\
        pseudo.o       	\
	repeat.o	\
        scanner.o	\
        segment.o       \
        segrange.o      \
        sizeof.o        \
        spool.o         \
        struct.o        \
        studyexpr.o     \
        symentry.o      \
        symbol.o        \
        symtab.o	\
        token.o         \
       	toklist.o      	\
      	ulabel.o

# -----------------------------------------------------------------------------
# List of all macro files

INCS =	atari.inc       \
        cbm.inc		\
      	cpu.inc		\
      	generic.inc	\
        longbranch.inc

LIBS = $(COMMON)/common.a

# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:  	$(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(INCS) $(OBJS) $(LIBS)
	$(CC) $(OBJS) $(LIBS) $(LDFLAGS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

inc:	$(INCS)

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(EXE) $(INCS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM -MG $^ > .depend

# -----------------------------------------------------------------------------
# Rules to make config includes

atari.inc:	macpack/atari.mac
	@$(CVT) $< $@ MacAtari

cbm.inc:	macpack/cbm.mac
	@$(CVT) $< $@ MacCBM

cpu.inc:	macpack/cpu.mac
	@$(CVT) $< $@ MacCPU

generic.inc:   	macpack/generic.mac
	@$(CVT) $< $@ MacGeneric

longbranch.inc:	macpack/longbranch.mac
	@$(CVT) $< $@ MacLongBranch










