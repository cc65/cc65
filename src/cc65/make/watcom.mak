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
	check.obj	\
	codegen.obj	\
	ctrans.obj	\
	datatype.obj	\
	declare.obj	\
	error.obj	\
	expr.obj 	\
	funcdesc.obj	\
	function.obj	\
	global.obj	\
	goto.obj	\
	ident.obj	\
	include.obj	\
	io.obj		\
	litpool.obj	\
	locals.obj	\
	loop.obj	\
	macrotab.obj	\
	main.obj	\
	mem.obj		\
	optimize.obj	\
	pragma.obj	\
	preproc.obj	\
	stmt.obj	\
	scanner.obj	\
	stdfunc.obj	\
	symentry.obj	\
	symtab.obj	\
	util.obj


.PRECIOUS $(OBJS:.obj=.c)

# ------------------------------------------------------------------------------
# Main targets

all:		cc65

cc65:		cc65.exe


# ------------------------------------------------------------------------------
# Other targets


cc65.exe:	$(OBJS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE anonname.obj
FILE asmcode.obj
FILE asmlabel.obj
FILE asmline.obj
FILE check.obj
FILE codegen.obj
FILE ctrans.obj
FILE datatype.obj
FILE declare.obj
FILE error.obj
FILE expr.obj
FILE funcact.obj
FILE funcdesc.obj
FILE function.obj
FILE global.obj
FILE goto.obj
FILE ident.obj
FILE include.obj
FILE io.obj
FILE litpool.obj
FILE locals.obj
FILE loop.obj
FILE macrotab.obj
FILE main.obj
FILE mem.obj
FILE optimize.obj
FILE pragma.obj
FILE preproc.obj
FILE stmt.obj
FILE scanner.obj
FILE stdfunc.obj
FILE symentry.obj
FILE symtab.obj
FILE util.obj
|

