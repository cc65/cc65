#
# ar65 Makefile for the Watcom compiler
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

OBJS =	add.obj		\
	del.obj		\
	error.obj	\
	exports.obj	\
	extract.obj	\
	fileio.obj	\
	global.obj	\
	library.obj	\
	list.obj	\
	main.obj	\
	objdata.obj	\
	objfile.obj

LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:		ar65

ar65:		ar65.exe


# ------------------------------------------------------------------------------
# Other targets


ar65.exe:	$(OBJS) $(LIBS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE add.obj
FILE del.obj
FILE error.obj
FILE exports.obj
FILE extract.obj
FILE fileio.obj
FILE global.obj
FILE library.obj
FILE list.obj
FILE main.obj
FILE mem.obj
FILE objdata.obj
FILE objfile.obj
LIBRARY ..\common\common.lib
|


clean:
	@if exist *.obj del *.obj
	@if exist ar65.exe del ar65.exe

strip:
       	@-wstrip ar65.exe

