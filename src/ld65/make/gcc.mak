#
# gcc Makefile for ld65
#

# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_LIB=\"/usr/lib/cc65/lib/\"
CFLAGS = -g -O2 -Wall $(CDEFS)
CC=gcc
LDFLAGS=

OBJS = 	bin.o		\
	binfmt.o	\
	config.o	\
	dbgsyms.o	\
	error.o		\
	exports.o	\
	expr.o		\
	extsyms.o	\
	fileio.o	\
	global.o        \
	library.o	\
	main.o	       	\
	mapfile.o	\
        mem.o		\
	o65.o		\
	objdata.o	\
	objfile.o	\
	scanner.o	\
	segments.o	\
	target.o

LIBS = ../common/common.a


EXECS = ld65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif



ld65:   $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *~ core *.map

zap:	clean
	rm -f *.o $(EXECS) .depend

	
# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -MM $^ > .depend


