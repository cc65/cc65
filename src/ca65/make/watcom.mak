#
# CA65 Makefile for the Watcom compiler
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

OBJS = 	condasm.obj	\
	ea.obj		\
	error.obj	\
	expr.obj	\
	fragment.obj	\
	global.obj	\
	incpath.obj	\
       	instr.obj	\
	istack.obj	\
	listing.obj	\
	macpack.obj	\
	macro.obj	\
	main.obj	\
	nexttok.obj	\
	objcode.obj	\
	objfile.obj	\
	options.obj	\
	pseudo.obj	\
	scanner.obj	\
	strexpr.obj	\
	symtab.obj	\
	toklist.obj	\
	ulabel.obj

LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:		ca65

ca65:		ca65.exe


# ------------------------------------------------------------------------------
# Other targets


ca65.exe:	$(OBJS) $(LIBS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE condasm.obj
FILE ea.obj
FILE error.obj
FILE expr.obj
FILE fragment.obj
FILE global.obj
FILE incpath.obj
FILE instr.obj
FILE istack.obj
FILE listing.obj
FILE macpack.obj
FILE macro.obj
FILE main.obj
FILE nexttok.obj
FILE objcode.obj
FILE objfile.obj
FILE options.obj
FILE pseudo.obj
FILE scanner.obj
FILE strexpr.obj
FILE symtab.obj
FILE toklist.obj
FILE ulabel.obj
LIBRARY ..\common\common.lib
|

clean:
	@if exist *.obj del *.obj
	@if exist ca65.exe del ca65.exe

strip:
	@-wstrip ca65.exe

