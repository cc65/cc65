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

LIB	= common.lib

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

OBJS =	bitops.obj	\
	hashstr.obj	\
	wildargv.obj	\
	xsprintf.obj
			

.PRECIOUS $(OBJS:.obj=.cc) $(LIB)

# ------------------------------------------------------------------------------
# Main targets

all:	lib

lib:	$(LIB)

$(LIB): $(OBJS)
	@echo Creating library...
	&@$(AR) -q -b -P=32 $(LIB) +-$?
	@echo Done!

clean:
	@if exist *.obj del *.obj
	@if exist $(LIB) del $(LIB)



