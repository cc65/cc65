#
# CC65 Makefile for the Watcom compiler
#

# ------------------------------------------------------------------------------
# Generic stuff

.AUTODEPEND
.SUFFIXES	.ASM .C .CC .CPP
.SWAP

AR	= WLIB
LD	= WLINK

!if !$d(TARGET)
!if $d(__OS2__)
TARGET = OS2
!else
TARGET = NT
!endif
!endif

# target specific macros.
!if $(TARGET)==OS2

# --------------------- OS2 ---------------------
SYSTEM = os2v2
CC = WCC386
CCCFG  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2

!elif $(TARGET)==DOS32

# -------------------- DOS4G --------------------
SYSTEM = dos4g
CC = WCC386
CCCFG  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2

!elif $(TARGET)==DOS

# --------------------- DOS ---------------------
SYSTEM = dos
CC = WCC
CCCFG  = -bt=$(TARGET) -d1 -onatx -zp2 -2 -ml -zq -w2

!elif $(TARGET)==NT

# --------------------- NT ----------------------
SYSTEM = nt
CC = WCC386
CCCFG  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2

!else
!error
!endif

# Add the include dir
CCCFG	= $(CCCFG) -i=..\common


# ------------------------------------------------------------------------------
# Implicit rules

.c.obj:
  $(CC) $(CCCFG) $<


# ------------------------------------------------------------------------------
# All library OBJ files

OBJS =	anonname.obj	\
     	asmcode.obj	\
	asmlabel.obj	\
	asmstmt.obj	\
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
	coptpush.obj	\
        coptsize.obj    \
	coptstop.obj	\
        coptstore.obj   \
	coptsub.obj	\
	copttest.obj	\
	cpu.obj		\
	dataseg.obj	\
	datatype.obj	\
	declare.obj	\
	declattr.obj	\
	error.obj	\
	expr.obj	\
	exprheap.obj	\
	exprnode.obj	\
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
	locals.obj	\
	loop.obj 	\
	macrotab.obj	\
	main.obj	\
	opcodes.obj	\
	preproc.obj	\
	pragma.obj	\
	reginfo.obj	\
    	scanner.obj	\
        scanstrbuf.obj  \
	segments.obj	\
    	stdfunc.obj	\
    	stmt.obj	\
	swstmt.obj	\
    	symentry.obj	\
    	symtab.obj	\
	textseg.obj	\
    	typecmp.obj	\
	util.obj

LIBS = ..\common\common.lib

.PRECIOUS $(OBJS:.obj=.c)

# ------------------------------------------------------------------------------
# Main targets

all:		cc65

cc65:		cc65.exe


# ------------------------------------------------------------------------------
# Other targets


cc65.exe:	$(OBJS) $(LIBS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE anonname.obj
FILE asmcode.obj
FILE asmlabel.obj
FILE asmstmt.obj
FILE casenode.obj
FILE codeent.obj
FILE codegen.obj
FILE codelab.obj
FILE codeinfo.obj
FILE codeopt.obj
FILE codeseg.obj
FILE compile.obj
FILE coptadd.obj
FILE coptc02.obj
FILE coptcmp.obj
FILE coptind.obj
FILE coptneg.obj
FILE coptpush.obj
FILE coptsize.obj
FILE coptstop.obj
FILE coptstore.obj
FILE coptsub.obj
FILE copttest.obj
FILE cpu.obj
FILE dataseg.obj
FILE datatype.obj
FILE declare.obj
FILE declattr.obj
FILE error.obj
FILE expr.obj
FILE exprheap.obj
FILE exprnode.obj
FILE funcdesc.obj
FILE function.obj
FILE global.obj
FILE goto.obj
FILE hexval.obj
FILE ident.obj
FILE incpath.obj
FILE input.obj
FILE lineinfo.obj
FILE litpool.obj
FILE locals.obj
FILE loop.obj
FILE macrotab.obj
FILE main.obj
FILE opcodes.obj
FILE preproc.obj
FILE pragma.obj
FILE reginfo.obj
FILE scanner.obj
FILE scanstrbuf.obj
FILE segments.obj
FILE stdfunc.obj
FILE stmt.obj
FILE swstmt.obj
FILE symentry.obj
FILE symtab.obj
FILE textseg.obj
FILE typecmp.obj
FILE util.obj
LIBRARY ..\common\common.lib
|

clean:
	@if exist *.obj del *.obj
       	@if exist cc65.exe del cc65.exe

strip:
	@-wstrip cc65.exe


