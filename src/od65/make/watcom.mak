#
# OD65 Makefile for the Watcom compiler
#
  
# ------------------------------------------------------------------------------
# Generic stuff

.AUTODEPEND
.SUFFIXES	.ASM .C .CC .CPP
.SWAP

AR  	= WLIB
LD  	= WLINK

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

OBJS = 	dump.obj	\
	error.obj	\
	fileio.obj 	\
	global.obj  	\
	main.obj

LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:		od65

od65:		od65.exe


# ------------------------------------------------------------------------------
# Other targets


od65.exe:	$(OBJS) $(LIBS)
  	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE dump.obj
FILE error.obj
FILE fileio.obj
FILE global.obj
FILE main.obj
LIBRARY ..\common\common.lib
|

clean:
     	@if exist *.obj del *.obj
       	@if exist od65.exe del od65.exe

strip:
	@-wstrip od65.exe

