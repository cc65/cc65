#
# Makefile for cross-compiler version of CC65.
#


# Library dir
COMMON	= ../common

# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_INC=\"/usr/lib/cc65/include/\"
CFLAGS = -O2 -g -Wall -I$(COMMON) $(CDEFS)
CC=gcc
LDFLAGS=

OBJS =	anonname.o	\
	asmcode.o	\
	asmlabel.o	\
	asmline.o	\
	codegen.o	\
	compile.o	\
	cpu.o		\
	datatype.o	\
	declare.o      	\
	declattr.o	\
	error.o	 	\
	expr.o	 	\
	exprheap.o     	\
	exprnode.o	\
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
	segname.o	\
	stdfunc.o	\
	stmt.o 		\
	symentry.o	\
	symtab.o       	\
	typecmp.o	\
	util.o

LIBS = $(COMMON)/common.a

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
	$(CC) -I$(COMMON) -MM $^ > .depend


