#
# Makefile for the debug info test executable
#




# ------------------------------------------------------------------------------

# The executable to build
EXE  	= dbgsh

# Library dir
COMMON	= ../common

#
CC      = gcc
CFLAGS  = -g -Wall -W -I$(COMMON)
EBIND   = emxbind
LDFLAGS =

# ------------------------------------------------------------------------------
# Object files to link

OBJS = 	dbginfo.o     	\
       	dbgsh.o

LIBS = $(COMMON)/common.a

# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:   	$(EXE)
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
	$(RM) $(OBJS) $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -MM $^ > .depend



