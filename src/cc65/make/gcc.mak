#
# Makefile for cross-compiler version of CC65.
#



# ------------------------------------------------------------------------------

# The executable to build
EXE  	= cc65

# Library directories
COMMON	= ../common

# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_INC=\"/usr/lib/cc65/include/\"
CFLAGS = -O2 -g -Wall -W -I$(COMMON) $(CDEFS)
CC=gcc
EBIND=emxbind
LDFLAGS=

# ------------------------------------------------------------------------------
# Object files and libraries to link

OBJS =	anonname.o	\
     	asmcode.o 	\
	asmlabel.o	\
	asmstmt.o	\
	codeent.o	\
	codegen.o 	\
	codelab.o	\
	codeinfo.o	\
	codeopt.o	\
	codeseg.o	\
	compile.o 	\
	coptadd.o	\
	coptcmp.o	\
	coptind.o	\
	coptneg.o	\
	coptstop.o	\
	coptsub.o	\
	copttest.o	\
	cpu.o	  	\
	dataseg.o	\
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
	lineinfo.o	\
	litpool.o      	\
	locals.o	\
	loop.o		\
	macrotab.o	\
	main.o		\
	opcodes.o	\
	preproc.o      	\
	pragma.o       	\
	reginfo.o	\
    	scanner.o      	\
	segments.o	\
    	stdfunc.o	\
    	stmt.o 		\
	swstmt.o	\
    	symentry.o	\
    	symtab.o       	\
	textseg.o	\
    	typecmp.o	\
    	util.o

LIBS =	$(COMMON)/common.a


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

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) -o $(EXE) $(CFLAGS) $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

clean:
	rm -f *~ core *.map

zap:	clean
	rm -f *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM $^ > .depend



