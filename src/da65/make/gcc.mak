#
# gcc Makefile for da65
#

# Library dir
COMMON	= ../common

CFLAGS = -g -O2 -Wall -I$(COMMON)
CC=gcc
LDFLAGS=

OBJS = 	attrtab.o	\
	code.o		\
	cpu.o		\
	error.o		\
	global.o	\
  	handler.o	\
	main.o		\
  	opctable.o	\
	output.o	

LIBS = $(COMMON)/common.a


EXECS = da65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
  	@$(MAKE) -f make/gcc.mak all
endif



da65:   $(OBJS) $(LIBS)
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
	$(CC) -I$(COMMON) -MM $^ > .depend


