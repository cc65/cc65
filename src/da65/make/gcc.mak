#
# gcc Makefile for da65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= da65

# Library dir
COMMON	= ../common

#
CC      = gcc
CFLAGS  = -g -O2 -Wall -W -std=c89 
CFLAGS += -I$(COMMON)
EBIND   = emxbind
LDFLAGS =
        

# -----------------------------------------------------------------------------
# List of all object files

OBJS = 	asminc.o        \
        attrtab.o	\
	code.o	 	\
        comments.o      \
	data.o		\
	error.o	 	\
	global.o 	\
  	handler.o	\
	infofile.o	\
        labels.o        \
	main.o	 	\
        opc6502.o       \
        opc65816.o      \
        opc65c02.o      \
        opc65sc02.o     \
  	opctable.o	\
	output.o 	\
	scanner.o       \
        segment.o

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


