#
# Makefile for the cl65 compile&link utility
#

# Library dir
COMMON	= ../common

CC=gcc
CFLAGS = -O2 -g -Wall -W -I$(COMMON)
EBIND  = emxbind
LDFLAGS=

OBJS =	error.o	 	\
	global.o 	\
	main.o		\
	spawn-unix.o

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
	$(CC) $(LDFLAGS) -o cl65 $(CFLAGS) $(OBJS) $(LIBS)
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
	$(CC) -I$(COMMON) -MM $^ > .depend



