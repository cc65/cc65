#
# da65 Makefile for the Watcom compiler
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
# All OBJ files

OBJS = 	attrtab.obj	\
	code.obj       	\
	config.obj 	\
	cpu.obj	   	\
	data.obj	\
	error.obj  	\
	global.obj 	\
  	handler.obj	\
	main.obj   	\
  	opctable.obj	\
	output.obj 	\
	scanner.obj


LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:	      	da65

da65:	      	da65.exe


# ------------------------------------------------------------------------------
# Other targets


da65.exe:     	$(OBJS) $(LIBS)
  	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE attrtab.obj
FILE code.obj
FILE config.obj
FILE cpu.obj
FILE data.obj
FILE error.obj
FILE global.obj
FILE handler.obj
FILE main.obj
FILE opctable.obj
FILE output.obj
FILE scanner.obj
LIBRARY ..\common\common.lib
|

clean:
	@if exist *.obj del *.obj
	@if exist *.obj del da65.exe

strip:
	@-wstrip da65.exe

