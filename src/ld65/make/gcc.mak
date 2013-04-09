#
# gcc Makefile for ld65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= ld65

# Library dir
COMMON	= ../common

# Several search paths. You may redefine these on the command line
LD65_LIB = \"/usr/lib/cc65/lib/\"
LD65_OBJ = \"/usr/lib/cc65/lib/\"
LD65_CFG = \"/usr/lib/cc65/cfg/\"

#
CC      = gcc
CFLAGS  = -g -O2 -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
override CFLAGS += -DLD65_LIB=$(LD65_LIB) -DLD65_OBJ=$(LD65_OBJ) -DLD65_CFG=$(LD65_CFG)
EBIND   = emxbind
LDFLAGS =

# -----------------------------------------------------------------------------
# List of all object files

OBJS = 	asserts.o       \
	bin.o		\
	binfmt.o	\
	cfgexpr.o       \
	condes.o	\
	config.o	\
	dbgfile.o       \
	dbgsyms.o	\
	error.o		\
	exports.o	\
	expr.o		\
	extsyms.o	\
	fileinfo.o	\
	fileio.o	\
	filepath.o      \
	fragment.o	\
	global.o        \
	library.o	\
	lineinfo.o	\
	main.o	       	\
	mapfile.o	\
        memarea.o       \
	o65.o		\
	objdata.o	\
	objfile.o	\
	scanner.o	\
        scopes.o        \
	segments.o	\
        span.o          \
	spool.o         \
	tgtcfg.o        \
        tpool.o

# -----------------------------------------------------------------------------
# List of all config includes

LIBS = $(COMMON)/common.a


# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXE)
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
	$(RM) *.o $(INCS) $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $(OBJS:.o=.c) > .depend
