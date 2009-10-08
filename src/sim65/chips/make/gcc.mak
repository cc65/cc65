#
# gcc Makefile for the sim65 chip plugins
#

# Include directories
COMMON  = ../../common
SIM65	= ..

CFLAGS 	= -g -O2 -Wall -W -std=c89
override CFLAGS += -I$(COMMON) -I$(SIM65) -fpic
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

#LIBS 	= $(COMMON)/common.a

CHIPS  	=      	console.so      \
                ram.so		\
		rom.so		\
		stdio.so        \
		vic2.so

OBJS	= $(CHIPS:.so=.o)

#----------------------------------------------------------------------------
# Build rules

%.obj:  %.c
	$(CC) $(CFLAGS) $^

%.so:	%.o
	$(CC) $(CFLAGS) -shared -o $@ $(LIBS) $^ -L /usr/X11R6/lib -lX11
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

#----------------------------------------------------------------------------

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(CHIPS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


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
	$(CC) $(CFLAGS) -MM $^ > .depend


