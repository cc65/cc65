#
# Makefile for the cl65 compile&link utility
#

# Library dir
COMMON	= ../common

# Type of spawn function to use
SPAWN   = SPAWN_UNIX


CC=gcc
CFLAGS = -O2 -g -Wall -W -I$(COMMON) -D$(SPAWN)
EBIND  = emxbind
LDFLAGS=

OBJS =	error.o	 	\
	global.o 	\
	main.o

LIBS = $(COMMON)/common.a

EXECS = cl65


.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


cl65:	$(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) -o cl65 $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) cl65 ; fi

clean:
	rm -f *~ core

zap:	clean
	rm -f *.o $(EXECS) .depend


# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -D$(SPAWN) -MM $^ > .depend



