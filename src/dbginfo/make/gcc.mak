#
# Makefile for the debug info test executable
#




# ------------------------------------------------------------------------------

# The executable to build
EXE  	= dbgtest

#
CC      = gcc
CFLAGS  = -g -O2 -Wall -W
EBIND   = emxbind
LDFLAGS =

# ------------------------------------------------------------------------------
# Object files to link

OBJS = 	dbginfo.o     	\
	dbgtest.o


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

$(EXE):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
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



