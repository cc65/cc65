#
# Makefile for cross-compiler version of CC65.
#



# ------------------------------------------------------------------------------

# The executable to build
EXE  	= cc65

# Library directories
COMMON	= ../common
B6502	= b6502

# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_INC=\"/usr/lib/cc65/include/\"
CFLAGS = -O2 -g -Wall -I$(COMMON) -I$(B6502) $(CDEFS)
CC=gcc
EBIND=emxbind
LDFLAGS=

# ------------------------------------------------------------------------------
# Object files and libraries to link

OBJS =	anonname.o	\
     	asmcode.o 	\
	asmlabel.o	\
	codegen.o 	\
	compile.o 	\
	cpu.o	  	\
	datatype.o	\
	declare.o      	\
	declattr.o	\
	error.o	  	\
	expr.o	 	\
	exprheap.o     	\
	exprnode.o	\
	funcdesc.o	\
	function.o     	\
	global.o 	\
	goto.o	   	\
	ident.o		\
	incpath.o      	\
	input.o		\
	litpool.o      	\
	locals.o	\
	loop.o		\
	macrotab.o	\
	main.o		\
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

LIBS =	$(B6502)/b6502.a	\
	$(COMMON)/common.a


# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(OBJS) $(LIBS) subs
	$(CC) $(LDFLAGS) -o $(EXE) $(CFLAGS) $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

.PHONY:	subs
subs:
	@for dir in $(COMMON) $(B6502); do   	     	\
	    $(MAKE) -C $$dir -f make/gcc.mak || exit 1;	\
	done						

clean:
	rm -f *~ core *.map

zap:	clean
	rm -f *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -I$(B6502) -MM $^ > .depend


