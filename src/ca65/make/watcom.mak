#
# CA65 Makefile for the Watcom compiler (using GNU make)
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

OBJS = 	condasm.obj	\
	dbginfo.obj	\
	ea.obj		\
	error.obj	\
	expr.obj	\
	feature.obj	\
	filetab.obj	\
	fragment.obj	\
	global.obj	\
	incpath.obj	\
       	instr.obj	\
	istack.obj	\
	lineinfo.obj	\
	listing.obj	\
	macpack.obj	\
	macro.obj	\
	main.obj	\
	nexttok.obj	\
	objcode.obj	\
	objfile.obj	\
	options.obj	\
	pseudo.obj	\
	repeat.obj	\
	scanner.obj	\
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
	@echo DEBUG ALL > $(LNKCFG)
	@echo OPTION QUIET >> $(LNKCFG)
	@echo NAME $@ >> $(LNKCFG)
	@for %%i in ($(OBJS)) do echo FILE %%i >> $(LNKCFG)
	@for %%i in ($(LIBS)) do echo LIBRARY %%i >> $(LNKCFG)
	$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)

clean:
	@if exist *.obj del *.obj
	@if exist ca65.exe del ca65.exe

strip:
	@-wstrip ca65.exe

