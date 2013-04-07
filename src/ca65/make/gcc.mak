#
# gcc Makefile for ca65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= ca65

# Library dir
COMMON	= ../common

# Several search paths. You may redefine these on the command line
CA65_INC = \"/usr/lib/cc65/asminc/\"

#
CC	= gcc
CFLAGS 	= -g -O2 -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
override CFLAGS += -DCA65_INC=$(CA65_INC)
EBIND	= emxbind
LDFLAGS	=

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
	macro.o	       	\
	main.o 	       	\
	nexttok.o      	\
	objcode.o      	\
	objfile.o      	\
	options.o      	\
	pseudo.o       	\
	repeat.o	\
	scanner.o	\
        segdef.o        \
	segment.o       \
	sizeof.o        \
	span.o          \
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

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(INCS) $(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $(OBJS:.o=.c) > .depend
