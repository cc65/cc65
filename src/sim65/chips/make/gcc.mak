#
# gcc Makefile for the sim65 chip plugins
#

# Include directories
COMMON  = ../../common
SIM65	= ..

CFLAGS 	= -g -O2 -Wall -W -I$(COMMON) -I$(SIM65) -fpic
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

LIBS 	= $(COMMON)/common.a

CHIPS  	=      	ram.so		\
		rom.so		\
		stdio.so

OBJS	= $(CHIPS:.so=.o)

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(CHIPS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


# Rules to make chips

ram.so:         ram.o
	$(CC) $(CFLAGS) -shared -o $@ $(LIBS) $^
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

rom.so:         rom.o
	$(CC) $(CFLAGS) -shared -o $@ $(LIBS) $^
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

stdio.so:	stdio.o
	$(CC) $(CFLAGS) -shared -o $@ $(LIBS) $^
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

# Admin stuff

clean:
	rm -f *~ core *.lst

zap:	clean
	rm -f *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(CHIPS:.so=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -I$(SIM65) -MM $^ > .depend


