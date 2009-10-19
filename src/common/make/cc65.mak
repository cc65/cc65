#
# cc65 Makefile for the cc65 common directory
#


RM	= rm -f
SYS     = c64
CFLAGS 	= -g -T -t $(SYS) -Oirs --standard c89
AS	= ../ca65/ca65
AR      = ../ar65/ar65
CC	= ../cc65/cc65
LDFLAGS	=
LIB	= common.lib



# --------------------------------------------------------------------------
# Generic rules

.c.o:
	@echo $<
	@$(CC) $(CFLAGS) $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(MY_ASM) -t $(SYS) $<

.o:
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^ $(CLIB)


# --------------------------------------------------------------------------
# Object files for the library

OBJS =	abend.o		\
	addrsize.o      \
        assertion.o     \
	bitops.o	\
	chartype.o	\
	check.o		\
	cmdline.o	\
	coll.o		\
	cpu.o           \
	debugflag.o     \
	exprdefs.o	\
	filepos.o	\
	filetype.o      \
	fname.o		\
	fp.o            \
	hashstr.o	\
	hashtab.o       \
	intstack.o      \
	matchpat.o      \
	mmodel.o        \
	print.o		\
	searchpath.o    \
	segdefs.o       \
	segnames.o      \
	shift.o         \
	strbuf.o	\
	strpool.o       \
	strstack.o      \
	strutil.o	\
	target.o	\
	tgttrans.o	\
	version.o       \
	xmalloc.o	\
	xsprintf.o


# ------------------------------------------------------------------------------
# Dummy targets

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	lib
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

.PHONY:	lib
lib:	$(LIB)

$(LIB):	$(OBJS)
	$(AR) a $(LIB) $?

clean:
	$(RM) *~ core *.map

zap:	clean
	$(RM) *.o $(LIB) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $^ > .depend


