#
# ld65 Makefile for the Watcom compiler
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

OBJS =	bin.obj		\
	binfmt.obj	\
	condes.obj	\
	config.obj	\
	dbginfo.obj	\
	dbgsyms.obj	\
	error.obj	\
	exports.obj	\
	expr.obj	\
	extsyms.obj	\
	fileinfo.obj	\
	fileio.obj	\
	fragment.obj	\
	global.obj	\
	library.obj	\
	lineinfo.obj	\
	main.obj	\
	mapfile.obj	\
	o65.obj		\
	objdata.obj	\
	objfile.obj	\
	scanner.obj	\
	segments.obj	\
	tgtcfg.obj

LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:	      	ld65

ld65:	      	ld65.exe


# ------------------------------------------------------------------------------
# Other targets


ld65.exe:     	$(OBJS) $(LIBS)
	$(LD) system $(SYSTEM) @&&|
DEBUG ALL
OPTION QUIET
NAME $<
FILE bin.obj
FILE binfmt.obj
FILE condes.obj
FILE config.obj
FILE dbginfo.obj
FILE dbgsyms.obj
FILE error.obj
FILE exports.obj
FILE expr.obj
FILE extsyms.obj
FILE fileinfo.obj
FILE fileio.obj
FILE fragment.obj
FILE global.obj
FILE library.obj
FILE lineinfo.obj
FILE main.obj
FILE mapfile.obj
FILE o65.obj
FILE objdata.obj
FILE objfile.obj
FILE scanner.obj
FILE segments.obj
FILE tgtcfg.obj
LIBRARY ..\common\common.lib
|

clean:
	@if exist *.obj del *.obj
	@if exist *.obj del ld65.exe

strip:
	@-wstrip ld65.exe

