#
# gcc Makefile for ld65
#

# Library dir
COMMON	= ../common

# Default for the compiler lib search path as compiler define
CDEFS=-DCC65_LIB=\"/usr/lib/cc65/lib/\"
CFLAGS = -g -O2 -Wall -W -I$(COMMON) $(CDEFS)
CC=gcc
EBIND=emxbind
LDFLAGS=

# Perl script for config file conversion
CVT=cfg/cvt-cfg.pl

# -----------------------------------------------------------------------------
# List of all object files

OBJS = 	asserts.o       \
        bin.o		\
	binfmt.o	\
	condes.o	\
	config.o	\
        dbgfile.o       \
	dbginfo.o	\
	dbgsyms.o	\
	error.o		\
	exports.o	\
	expr.o		\
	extsyms.o	\
	fileinfo.o	\
	fileio.o	\
        filepath.o      \
	fragment.o	\
	global.o        \
	library.o	\
	lineinfo.o	\
	main.o	       	\
	mapfile.o	\
	o65.o		\
	objdata.o	\
	objfile.o	\
	scanner.o	\
	segments.o	\
        spool.o         \
	tgtcfg.o

# -----------------------------------------------------------------------------
# List of all config includes

INCS =	apple2.inc	\
	apple2enh.inc	\
	atari.inc	\
        atmos.inc       \
	bbc.inc		\
	c128.inc	\
	c16.inc		\
	c64.inc		\
	cbm510.inc	\
	cbm610.inc	\
	geos.inc	\
	lunix.inc	\
        module.inc      \
	nes.inc		\
	none.inc	\
	pet.inc		\
	plus4.inc	\
        supervision.inc \
	vic20.inc

# -----------------------------------------------------------------------------
#

LIBS = $(COMMON)/common.a


EXECS = ld65

.PHONY: all
ifeq (.depend,$(wildcard .depend))
all : $(EXECS)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif



ld65:   $(INCS) $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $@ ; fi

inc:	$(INCS)

clean:
	$(RM) *~ core *.map

zap:   	clean
	$(RM) *.o $(INCS) $(EXECS) .depend


# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) -I$(COMMON) -MM -MG $^ > .depend

# -----------------------------------------------------------------------------
# Rules to make config includes

apple2.inc:	cfg/apple2.cfg
	@$(CVT) $< $@ CfgApple2
				  
# The apple2enh target uses the same config as the apple2
apple2enh.inc:	cfg/apple2.cfg
	@$(CVT) $< $@ CfgApple2Enh

atari.inc:     	cfg/atari.cfg
	@$(CVT) $< $@ CfgAtari

atmos.inc:     	cfg/atmos.cfg
	@$(CVT) $< $@ CfgAtmos

bbc.inc:     	cfg/bbc.cfg
	@$(CVT) $< $@ CfgBBC

c16.inc:     	cfg/c16.cfg
	@$(CVT) $< $@ CfgC16

c64.inc:     	cfg/c64.cfg
	@$(CVT) $< $@ CfgC64

c128.inc:     	cfg/c128.cfg
	@$(CVT) $< $@ CfgC128

cbm510.inc:	cfg/cbm510.cfg
	@$(CVT) $< $@ CfgCBM510

cbm610.inc:	cfg/cbm610.cfg
	@$(CVT) $< $@ CfgCBM610

geos.inc:	cfg/geos.cfg
	@$(CVT) $< $@ CfgGeos

lunix.inc:	cfg/lunix.cfg
	@$(CVT) $< $@ CfgLunix

module.inc:	cfg/module.cfg
	@$(CVT) $< $@ CfgModule

nes.inc:	cfg/nes.cfg
	@$(CVT) $< $@ CfgNES

none.inc:	cfg/none.cfg
	@$(CVT) $< $@ CfgNone

pet.inc:       	cfg/pet.cfg
	@$(CVT) $< $@ CfgPET

plus4.inc:     	cfg/plus4.cfg
	@$(CVT) $< $@ CfgPlus4

supervision.inc:	cfg/supervision.cfg
	@$(CVT) $< $@ CfgSupervision

vic20.inc:     	cfg/vic20.cfg
	@$(CVT) $< $@ CfgVic20


