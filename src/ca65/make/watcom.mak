#
# CA65 Makefile for the Watcom compiler (using GNU make)
#

# ------------------------------------------------------------------------------
# Generic stuff

# Environment variables for the watcom compiler
export WATCOM  = c:\\watcom
export INCLUDE = $(WATCOM)\\h

# We will use the windows compiler under linux (define as empty for windows)
WINE = wine --

# Programs
AR     	= $(WINE) WLIB
CC      = $(WINE) WCC386
LD     	= $(WINE) WLINK
WSTRIP	= $(WINE) WSTRIP

LNKCFG  = ld.tmp

# Program arguments
CFLAGS  = -d1 -onatx -zp4 -5 -zq -w2 -i=..\\common

# Target files
EXE	= ca65.exe

# Create NT programs by default
ifndef TARGET
TARGET = NT
endif

# --------------------- OS2 ---------------------
ifeq ($(TARGET),OS2)
SYSTEM  = os2v2
CFLAGS  += -bt=$(TARGET)
endif

# -------------------- DOS4G --------------------
ifeq ($(TARGET),DOS32)
SYSTEM  = dos4g
CFLAGS  += -bt=$(TARGET)
endif

# --------------------- NT ----------------------
ifeq ($(TARGET),NT)
SYSTEM  = nt
CFLAGS  += -bt=$(TARGET)
endif

# ------------------------------------------------------------------------------
# Implicit rules

%.obj:  %.c
	$(CC) $(CFLAGS) $^


# ------------------------------------------------------------------------------
# All library OBJ files

OBJS = 	anonname.obj    \
        asserts.obj     \
        condasm.obj	\
	dbginfo.obj	\
	ea.obj		\
        enum.obj        \
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
        segment.obj     \
        sizeof.obj      \
        spool.obj       \
        struct.obj      \
        studyexpr.obj   \
	symbol.obj	\
        symentry.obj    \
	symtab.obj	\
	toklist.obj	\
	ulabel.obj

LIBS = ../common/common.lib


# ------------------------------------------------------------------------------
# Main targets

all:	  	$(EXE)


# ------------------------------------------------------------------------------
# Other targets


$(EXE): 	$(OBJS) $(LIBS)
	@echo "DEBUG ALL" > $(LNKCFG)
	@echo "OPTION QUIET" >> $(LNKCFG)
	@echo "NAME $@" >> $(LNKCFG)
	@for i in $(OBJS); do echo "FILE $${i}"; done >> $(LNKCFG)
	@for i in $(LIBS); do echo "LIBRARY $${i}"; done >> $(LNKCFG)
	@$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)

clean:
	@rm -f *~ core

zap:	clean
	@rm -f *.obj $(EXE)

strip:
	@-$(WSTRIP) $(EXE)

