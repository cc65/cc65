#
# ar65 Makefile for the Watcom compiler (using GNU make)
#

# ------------------------------------------------------------------------------
# Generic stuff

AR	= WLIB
LD	= WLINK
LNKCFG  = ld.tmp

# --------------------- OS2 ---------------------
ifeq ($(TARGET),OS2)
SYSTEM  = os2v2
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# -------------------- DOS4G --------------------
ifeq ($(TARGET),DOS32)
SYSTEM  = dos4g
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# --------------------- NT ----------------------
ifeq ($(TARGET),NT)
SYSTEM  = nt
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# Add the include dir
CFLAGS  += -i=..\common

# ------------------------------------------------------------------------------
# Implicit rules

%.obj:  %.c
	$(CC) $(CFLAGS) $^


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
	@echo DEBUG ALL > $(LNKCFG)
	@echo OPTION QUIET >> $(LNKCFG)
	@echo NAME $@ >> $(LNKCFG)
	@for %%i in ($(OBJS)) do echo FILE %%i >> $(LNKCFG)
	@for %%i in ($(LIBS)) do echo LIBRARY %%i >> $(LNKCFG)
	$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)


clean:
	@if exist *.obj del *.obj
	@if exist ar65.exe del ar65.exe

strip:
	@-wstrip ar65.exe

