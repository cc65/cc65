#
# gcc Makefile for ar65
#

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -I$(COMMON)
CC	= gcc
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


