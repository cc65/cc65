#
# CC65 Makefile for the Watcom compiler (using GNU make) and wine
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
LIB	= common.lib

# Program arguments
CFLAGS  = -d1 -onatx -zp4 -5 -zq -w2

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

OBJS =	abend.obj	\
	bitops.obj	\
	chartype.obj	\
	check.obj	\
	cmdline.obj	\
	coll.obj	\
        cpu.obj         \
        debugflag.obj   \
	exprdefs.obj	\
	filepos.obj	\
        filetype.obj    \
	fname.obj	\
	hashstr.obj	\
        hashtab.obj     \
        matchpat.obj    \
	print.obj	\
        searchpath.obj  \
        segdefs.obj     \
        segnames.obj    \
	strbuf.obj	\
        strpool.obj     \
	strutil.obj	\
	target.obj	\
	tgttrans.obj	\
	xmalloc.obj	\
	xsprintf.obj


# ------------------------------------------------------------------------------
# Main targets

all:	lib

lib:	$(LIB)

$(LIB): $(OBJS)
	@echo Creating library...
	$(AR) -q -b -P=32 $(LIB) $(foreach OBJ, $(OBJS), +-$(OBJ))
	@echo Done!

clean:
	@rm -f *~ core

zap:	clean
	@rm -f *.obj $(LIB)

strip:
	@true




