#
# gcc Makefile for the cc65 common directory
#

RM	= rm -f
AR	= ar
CFLAGS 	= -g -O2 -Wall -W -std=c89
CC	= gcc
LDFLAGS	=
LIB	= common.a


# ------------------------------------------------------------------------------
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
	hashfunc.o	\
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
	$(AR) rs $(LIB) $?

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


