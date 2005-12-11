#
# gcc Makefile for co65
#

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -W -std=c89 -I$(COMMON)
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

OBJS =	convert.o       \
        error.o         \
        fileio.o        \
        global.o        \
        main.o          \
        model.o         \
        o65.o

LIBS = $(COMMON)/common.a

EXECS = co65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif



co65:   $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

clean:
	$(RM) *~ core *.lst

zap:	clean
	$(RM) *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $^ > .depend


