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
	asmline.obj	\
	codegen.obj	\
	compile.obj	\
	cpu.obj	 	\
	ctrans.obj	\
	datatype.obj	\
	declare.obj	\
	error.obj	\
	expr.obj 	\
	exprheap.obj   	\
	exprnode.obj	\
	funcdesc.obj	\
	function.obj	\
	global.obj	\
	goto.obj	\
	ident.obj	\
	incpath.obj    	\
	input.obj	\
	litpool.obj	\
	locals.obj	\
	loop.obj	\
	macrotab.obj	\
	main.obj	\
	optimize.obj	\
	pragma.obj	\
	preproc.obj	\
	stmt.obj	\
	scanner.obj	\
	segname.obj	\
	stdfunc.obj	\
	symentry.obj	\
	symtab.obj	\
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
FILE asmline.obj
FILE codegen.obj
FILE compile.obj
FILE cpu.obj
FILE ctrans.obj
FILE datatype.obj
FILE declare.obj
FILE error.obj
FILE expr.obj
FILE exprheap.obj
FILE exprnode.obj
FILE funcdesc.obj
FILE function.obj
FILE global.obj
FILE goto.obj
FILE ident.obj
FILE incpath.obj
FILE input.obj
FILE litpool.obj
FILE locals.obj
FILE loop.obj
FILE macrotab.obj
FILE main.obj
FILE optimize.obj
FILE pragma.obj
FILE preproc.obj
FILE stmt.obj
FILE scanner.obj
FILE segname.obj
FILE stdfunc.obj
FILE symentry.obj
FILE symtab.obj
FILE typecmp.obj
FILE util.obj
LIBRARY ..\common\common.lib
|

clean:
	@if exist *.obj del *.obj
       	@if exist cc65.exe del cc65.exe

strip:
	@-wstrip cc65.exe


