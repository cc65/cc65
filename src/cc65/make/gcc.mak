#
# Makefile for cross-compiler version of CC65.
#


# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_INC=\"/usr/lib/cc65/include/\"
CFLAGS = -O2 -g -Wall $(CDEFS)
CC=gcc
LDFLAGS=

OBJS =	anonname.o	\
	asmcode.o	\
	asmlabel.o	\
	asmline.o	\
	check.o       	\
	codegen.o	\
	compile.o	\
	cpu.o		\
	ctrans.o       	\
	datatype.o	\
	declare.o      	\
	error.o	 	\
	expr.o	 	\
	funcdesc.o	\
	function.o     	\
	global.o 	\
	goto.o		\
	ident.o		\
	incpath.o      	\
	input.o		\
	litpool.o      	\
	locals.o	\
	loop.o		\
	macrotab.o	\
	main.o		\
	optimize.o     	\
	preproc.o      	\
	pragma.o       	\
	scanner.o      	\
	stdfunc.o	\
	stmt.o 		\
	symentry.o	\
	symtab.o       	\
	util.o

LIBS = ../common/common.a

EXECS = cc65


.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif


cc65:	$(OBJS)
	$(CC) $(LDFLAGS) -o cc65 $(CFLAGS) $(OBJS) $(LIBS)

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


