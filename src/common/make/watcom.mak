#
# CC65 Makefile for the Watcom compiler (using GNU make)
#

# ------------------------------------------------------------------------------
# Generic stuff

AR	= WLIB
LD	= WLINK
LIB	= common.lib


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
	exprdefs.obj	\
	filepos.obj	\
	fname.obj	\
	hashstr.obj	\
        matchpat.obj    \
	print.obj	\
        segdefs.obj     \
	strbuf.obj	\
	strutil.obj	\
	target.obj	\
	tgttrans.obj	\
	wildargv.obj	\
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
	@if exist *.obj del *.obj
	@if exist $(LIB) del $(LIB)





