#
# gcc Makefile for a65, link65 & libr65
#

CFLAGS 	= -g -O2 -Wall
CC	= gcc
LDFLAGS	=

OBJS =  condasm.o	\
	ea.o	      	\
        error.o	      	\
        expr.o	      	\
	fragment.o	\
        global.o       	\
	incpath.o	\
        instr.o	      	\
	istack.o	\
	listing.o	\
	macpack.o     	\
       	macro.o	      	\
        main.o 	      	\
	nexttok.o	\
        objcode.o     	\
        objfile.o	\
       	options.o	\
        pseudo.o	\
        scanner.o	\
	strexpr.o	\
        symtab.o	\
       	toklist.o      	\
	ulabel.o

LIBS = ../common/common.a

EXECS = ca65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif



ca65:   $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *~ core *.lst

zap:	clean
	rm -f *.o $(EXECS) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -MM $^ > .depend


