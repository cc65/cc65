#
# Makefile for the cl65 compile&link utility
#

CC=gcc
CFLAGS = -O2 -g -Wall
LDFLAGS=

OBJS =	error.o	 	\
	global.o 	\
	main.o		\
	mem.o		\
	spawn.o

EXECS = cl65


.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


cl65:	$(OBJS)
	$(CC) $(LDFLAGS) -o cl65 $(CFLAGS) $(OBJS)

clean:
	rm -f *~ core

zap:	clean
	rm -f *.o $(EXECS) .depend


# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -MM $^ > .depend



