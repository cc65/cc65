#
# Makefile for the cl65 compile&link utility
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= cl65

# Library dir
COMMON	= ../common

# Type of spawn function to use
SPAWN   = SPAWN_UNIX
ifneq ($(Kickstart),)
SPAWN   = SPAWN_AMIGA
endif

#
CC      = gcc
CFLAGS  = -O2 -g -Wall -W -std=c89
CFLAGS += -I$(COMMON)
CFLAGS += -D$(SPAWN)
EBIND   = emxbind
LDFLAGS =

OBJS =	error.o	 	\
	global.o 	\
	main.o

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
	$(CC) $^ $(LDFLAGS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -D$(SPAWN) -MM $^ > .depend



