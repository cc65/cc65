#
# gcc Makefile for the sim65 chip plugins
#

# Library dir
SIM65	= ..

CFLAGS 	= -g -O2 -Wall -W -I$(SIM65) -fpic
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

CHIPS  	=      	ram.so

OBJS	= $(CHIPS:.so=.o)

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(CHIPS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


ram.so:         ram.o
	$(CC) $(CFLAGS) -shared -o $@ $^
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

clean:
	rm -f *~ core *.lst

zap:	clean
	rm -f *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(CHIPS:.so=.c)
	@echo "Creating dependency information"
	$(CC) -I$(SIM65) -MM $^ > .depend


