#
# CL65 Makefile for the Watcom compiler
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
# All OBJ files

OBJS =	error.obj	\
	global.obj	\
	main.obj

.PRECIOUS $(OBJS:.obj=.c)

# ------------------------------------------------------------------------------
# Main targets

all:		cl65

cl65:		cl65.exe


# ------------------------------------------------------------------------------
# Other targets


cl65.exe:	$(OBJS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE error.obj
FILE global.obj
FILE main.obj
LIBRARY ..\common\common.lib
|


clean:
	@if exist *.obj del *.obj
       	@if exist cl65.exe del cl65.exe

strip:
	@-wstrip cl65.exe


