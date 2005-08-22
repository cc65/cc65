#
# gcc Makefile for ca65
#

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -W -I$(COMMON)
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
       	toklist.o      	\
	ulabel.o

# -----------------------------------------------------------------------------
# List of all macro files

INCS =	cbm.inc		\
	cpu.inc		\
	generic.inc	\
        longbranch.inc

# -----------------------------------------------------------------------------

LIBS = $(COMMON)/common.a

EXECS = ca65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

ca65:   $(INCS) $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

inc:	$(INCS)

clean:
	rm -f *~ core *.lst

zap:	clean
	rm -f *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM $^ > .depend

# -----------------------------------------------------------------------------
# Rules to make config includes

cbm.inc:	macpack/cbm.mac
	@$(CVT) $< $@ MacCBM

cpu.inc:	macpack/cpu.mac
	@$(CVT) $< $@ MacCPU

generic.inc:   	macpack/generic.mac
	@$(CVT) $< $@ MacGeneric

longbranch.inc:	macpack/longbranch.mac
	@$(CVT) $< $@ MacLongBranch










