#
# ld65 Makefile for the Watcom compiler (using GNU make)
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
# All OBJ files

OBJS =	asserts.obj     \
        bin.obj		\
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
        filepath.obj    \
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
        spool.obj       \
	tgtcfg.obj

LIBS = ..\common\common.lib


# ------------------------------------------------------------------------------
# Main targets

all:	      	ld65

ld65:	      	ld65.exe


# ------------------------------------------------------------------------------
# Other targets


ld65.exe:	$(OBJS) $(LIBS)
	@echo DEBUG ALL > $(LNKCFG)
	@echo OPTION QUIET >> $(LNKCFG)
	@echo NAME $@ >> $(LNKCFG)
	@for %%i in ($(OBJS)) do echo FILE %%i >> $(LNKCFG)
	@for %%i in ($(LIBS)) do echo LIBRARY %%i >> $(LNKCFG)
	$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)


clean:
	@if exist *.obj del *.obj
	@if exist *.obj del ld65.exe

strip:
	@-wstrip ld65.exe

