#
# Makefile for the od65 object file dump utility
#


# Library dir
COMMON	= ../common

CFLAGS = -O2 -g -Wall -I$(COMMON)
CC=gcc
LDFLAGS=

OBJS = 	dump.o		\
	error.o		\
	fileio.o	\
	global.o	\
	main.o

LIBS = $(COMMON)/common.a

EXE	= od65


.PHONY: all
ifeq (.depend,$(wildcard .depend))
all: 	$(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


$(EXE):	$(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $(CFLAGS) $(OBJS) $(LIBS)

clean:
	rm -f *~ core *.map

zap:	clean
	rm -f *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM $^ > .depend



