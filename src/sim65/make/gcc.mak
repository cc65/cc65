#
# gcc Makefile for sim65
#

# Library dir
COMMON	= ../common

CFLAGS 	= -g -O2 -Wall -W -I$(COMMON)
CC	= gcc
EBIND	= emxbind
LDFLAGS	=

OBJS = 	chip.o          \
        chiplib.o       \
        chippath.o      \
        cpucore.o     	\
	cputype.o     	\
        error.o         \
	global.o      	\
	main.o          \
        memory.o      	\
        simdata.o

LIBS = $(COMMON)/common.a

EXECS = sim65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXECS) chips
include .depend
else
all:	depend
 	@$(MAKE) -f make/gcc.mak all
endif


sim65:	$(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) -ldl
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

.PHONY:	chips
chips:
	@$(MAKE) -C chips -f make/gcc.mak


clean:
	rm -f *~ core *.lst

zap:  	clean
	rm -f *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM $^ > .depend


