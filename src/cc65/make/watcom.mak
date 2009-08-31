#
# CC65 Makefile for the Watcom compiler (using GNU make)
#

# ------------------------------------------------------------------------------
# Generic stuff

# Environment variables for the watcom compiler
export WATCOM  = c:\\watcom
export INCLUDE = $(WATCOM)\\h

# We will use the windows compiler under linux (define as empty for windows)
export WINEDEBUG=fixme-all
WINE = wine

# Programs
AR     	= $(WINE) WLIB
CC      = $(WINE) WCC386
LD     	= $(WINE) WLINK
WSTRIP	= $(WINE) WSTRIP -q

LNKCFG  = ld.tmp

# Program arguments
CFLAGS  = -d1 -onatx -zp4 -5 -zq -w2 -i=..\\common

# Target files
EXE	= cc65.exe

# Determine the svn version number if possible
ifneq "$(shell which svnversion 2>/dev/null)" ""
ifneq "$(wildcard .svn)" ""
SVNVERSION=$(shell svnversion)
else
SVNVERSION=unknown
endif
else
SVNVERSION=unknown
endif

# Create NT programs by default
ifndef TARGET
TARGET = NT
endif

# --------------------- OS2 ---------------------
ifeq ($(TARGET),OS2)
SYSTEM  = os2v2
CFLAGS  += -bt=$(TARGET)
endif

# -------------------- DOS4G --------------------
ifeq ($(TARGET),DOS32)
SYSTEM  = dos4g
CFLAGS  += -bt=$(TARGET)
endif

# --------------------- NT ----------------------
ifeq ($(TARGET),NT)
SYSTEM  = nt
CFLAGS  += -bt=$(TARGET)
endif

# ------------------------------------------------------------------------------
# Implicit rules

%.obj:  %.c
	$(CC) $(CFLAGS) $^


# ------------------------------------------------------------------------------
# All OBJ files

OBJS =	anonname.obj	\
     	asmcode.obj	\
	asmlabel.obj	\
	asmstmt.obj	\
        assignment.obj  \
	casenode.obj	\
	codeent.obj	\
	codegen.obj	\
	codelab.obj	\
	codeinfo.obj	\
	codeopt.obj	\
	codeseg.obj	\
	compile.obj	\
	coptadd.obj	\
	coptc02.obj	\
	coptcmp.obj	\
	coptind.obj	\
	coptneg.obj	\
        coptptrload.obj \
	coptpush.obj	\
        coptsize.obj    \
	coptstop.obj	\
        coptstore.obj   \
	coptsub.obj	\
	copttest.obj	\
	dataseg.obj	\
	datatype.obj	\
	declare.obj	\
	declattr.obj	\
	error.obj	\
	expr.obj 	\
        exprdesc.obj    \
	funcdesc.obj	\
	function.obj	\
	global.obj	\
	goto.obj	\
        hexval.obj      \
	ident.obj	\
	incpath.obj	\
	input.obj	\
	lineinfo.obj	\
	litpool.obj	\
        loadexpr.obj    \
	locals.obj	\
	loop.obj 	\
	macrotab.obj	\
	main.obj	\
	opcodes.obj	\
        output.obj      \
	preproc.obj	\
	pragma.obj	\
	reginfo.obj	\
    	scanner.obj	\
        scanstrbuf.obj  \
	segments.obj	\
        shiftexpr.obj   \
        stackptr.obj    \
        standard.obj    \
    	stdfunc.obj	\
        stdnames.obj    \
    	stmt.obj	\
	svnversion.obj	\
	swstmt.obj	\
    	symentry.obj	\
    	symtab.obj	\
        testexpr.obj    \
	textseg.obj	\
    	typecmp.obj	\
        typeconv.obj    \
	util.obj

LIBS = ../common/common.lib


# ------------------------------------------------------------------------------
# Main targets

all:	  	svnversion $(EXE)


# ------------------------------------------------------------------------------
# Other targets


$(EXE): 	$(OBJS) $(LIBS)
	@echo "DEBUG ALL" > $(LNKCFG)
	@echo "OPTION QUIET" >> $(LNKCFG)
	@echo "NAME $@" >> $(LNKCFG)
	@for i in $(OBJS); do echo "FILE $${i}"; done >> $(LNKCFG)
	@for i in $(LIBS); do echo "LIBRARY $${i}"; done >> $(LNKCFG)
	@$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)

.PHONY:	svnversion
svnversion:
	@$(RM) svnversion.c
	@echo "/* This file is auto-generated - do not modify! */" >> svnversion.c
	@echo "" >> svnversion.c
	@echo "const char SVNVersion[] = \"$(SVNVERSION)\";" >> svnversion.c

svnversion.c:	svnversion

clean:
	@rm -f *~ core

zap:	clean
	@rm -f $(OBJS) $(EXE) svnversion.c

strip:
	@-$(WSTRIP) $(EXE)

