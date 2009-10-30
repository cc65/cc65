#
# Makefile for the chrcvt vector font converter
#


# ------------------------------------------------------------------------------

# The executable to build
EXE  	= chrcvt

# Library dir
COMMON	= ../common

#
CC      = gcc
CFLAGS  = -O2 -g -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
EBIND   = emxbind
LDFLAGS =


# -----------------------------------------------------------------------------
# List of all object files

OBJS =  error.o		\
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
	$(CC) $(CFLAGS) -MM $^ > .depend



