#
# gcc Makefile for ar65
#

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -W -std=c89 -I$(COMMON)
CC	= gcc
EBIND	= emxbind
LDFLAGS	=


OBJS = 	add.o		\
    	del.o		\
    	error.o		\
    	exports.o	\
    	extract.o	\
    	fileio.o	\
    	global.o	\
    	library.o	\
    	list.o		\
    	main.o		\
    	objdata.o	\
    	objfile.o

LIBS = $(COMMON)/common.a


EXECS = ar65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif



ar65:   $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

clean:
	$(RM) *~ core

zap:	clean
	$(RM) *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $^ > .depend


