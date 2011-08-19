#
# gcc Makefile for ld65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= ld65

# Library dir
COMMON	= ../common

# Several search paths. You may redefine these on the command line
LD65_LIB = \"/usr/lib/cc65/lib/\"
LD65_OBJ = \"/usr/lib/cc65/lib/\"
LD65_CFG = \"/usr/lib/cc65/cfg/\"

#
CC      = gcc
CFLAGS  = -g -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
override CFLAGS += -DLD65_LIB=$(LD65_LIB) -DLD65_OBJ=$(LD65_OBJ) -DLD65_CFG=$(LD65_CFG)
EBIND   = emxbind
LDFLAGS =

# Perl script for config file conversion
CVT=cfg/cvt-cfg.pl

# -----------------------------------------------------------------------------
# List of all object files

OBJS = 	asserts.o       \
	bin.o		\
	binfmt.o	\
	cfgexpr.o       \
	condes.o	\
	config.o	\
	dbgfile.o       \
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
        memarea.o       \
	o65.o		\
	objdata.o	\
	objfile.o	\
	scanner.o	\
        scopes.o        \
	segments.o	\
        span.o          \
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
	lynx.inc	\
	module.inc      \
	nes.inc		\
	none.inc	\
	pet.inc		\
	plus4.inc	\
	supervision.inc \
	vic20.inc

LIBS = $(COMMON)/common.a


# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXE)
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(INCS) $(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

inc:	$(INCS)

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(INCS) $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(INCS) $(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $(OBJS:.o=.c) > .depend

# -----------------------------------------------------------------------------
# Rules to make config includes

apple2.inc:	cfg/apple2.cfg
	@$(CVT) $< $@ CfgApple2

apple2enh.inc:	cfg/apple2enh.cfg
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

lynx.inc:	cfg/lynx.cfg
	@$(CVT) $< $@ CfgLynx

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


