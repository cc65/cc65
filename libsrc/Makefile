# -*- make -*-
#
# Makefile for CC65 Libraries
#

.SUFFIXES: .o .obj .s .c

# Defines for executables. AR is used within this makefile, the others are
# passed to the submakes and contain paths relative to the subdirectories
# handled by the make subprocesses.
AR = ../src/ar65/ar65
AS = ../../src/ca65/ca65
CC = ../../src/cc65/cc65
LD = ../../src/ld65/ld65

# List of shared directories
SHAREDDIRS =	cbm		\
		common		\
		conio		\
		dbg		\
		em		\
		joystick	\
		mouse		\
		runtime		\
		serial		\
		tgi		\
		zlib

# List of all targets
ALLTARGETS =	apple2		\
		apple2enh	\
		atari		\
		atmos		\
		c128		\
		c16		\
		c64		\
		cbm510		\
		cbm610		\
		geos		\
		lynx		\
		nes		\
		pet		\
		plus4		\
		supervision	\
		vic20

#-----------------------------------------------------------------------------

.PHONY:	all
all:
	for tgt in $(ALLTARGETS); do			\
	    $(MAKE) mostly-clean "$$tgt"lib || exit 1;	\
	done

#-----------------------------------------------------------------------------
# Apple ][

.PHONY:	apple2lib
apple2lib:
	for i in runtime apple2 common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=apple2 -C $$i || exit 1; \
	    $(AR) a apple2.lib $$i/*.o || exit 1; \
	done
	cp apple2/apple2-auxmem.emd a2.auxmem.emd
	cp apple2/apple2-stdjoy.joy a2.stdjoy.joy
	cp apple2/apple2-stdmou.mou a2.stdmou.mou
	cp apple2/apple2-ssc.ser a2.ssc.ser
	cp apple2/apple2-280-192-8.tgi a2.hi.tgi
	cp apple2/apple2-40-48-16.tgi a2.lo.tgi
	if [ -d apple2/extra ]; then \
	    for i in apple2/extra/*.o; do \
	       	cp $$i apple2-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# enhanced Apple //e

.PHONY:	apple2enhlib
apple2enhlib:
	for i in runtime apple2enh common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=apple2enh -C $$i || exit 1; \
	    $(AR) a apple2enh.lib $$i/*.o || exit 1; \
	done
	cp apple2enh/apple2-auxmem.emd a2e.auxmem.emd
	cp apple2enh/apple2-stdjoy.joy a2e.stdjoy.joy
	cp apple2enh/apple2-stdmou.mou a2e.stdmou.mou
	cp apple2enh/apple2-ssc.ser a2e.ssc.ser
	cp apple2enh/apple2-280-192-8.tgi a2e.hi.tgi
	cp apple2enh/apple2-40-48-16.tgi a2e.lo.tgi
	if [ -d apple2enh/extra ]; then \
	    for i in apple2enh/extra/*.o; do \
	       	cp $$i apple2enh-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Atari

.PHONY:	atarilib
atarilib:
	for i in runtime atari common conio dbg em joystick tgi zlib; do \
	    $(MAKE) SYS=atari -C $$i || exit 1; \
	    $(AR) a atari.lib $$i/*.o || exit 1; \
	done
	cp atari/atari-stdjoy.joy ataristd.joy
	cp atari/atari-multijoy.joy atarimj8.joy
	cp atari/atari-10.tgi atari10.tgi
	cp atari/atari-10p2.tgi atr10p2.tgi
	cp atari/atari-11.tgi atari11.tgi
	cp atari/atari-14.tgi atari14.tgi
	cp atari/atari-15.tgi atari15.tgi
	cp atari/atari-15p2.tgi atr15p2.tgi
	cp atari/atari-3.tgi atari3.tgi
	cp atari/atari-4.tgi atari4.tgi
	cp atari/atari-5.tgi atari5.tgi
	cp atari/atari-6.tgi atari6.tgi
	cp atari/atari-7.tgi atari7.tgi
	cp atari/atari-8.tgi atari8.tgi
	cp atari/atari-8p2.tgi atr8p2.tgi
	cp atari/atari-9.tgi atari9.tgi
	cp atari/atari-9p2.tgi atr9p2.tgi
	if [ -d atari/extra ]; then \
	    for i in atari/extra/*.o; do \
	       	cp $$i atari-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Oric Atmos

.PHONY:	atmoslib
atmoslib:
	for i in runtime atmos common conio em joystick tgi zlib; do \
	    $(MAKE) SYS=atmos -C $$i || exit 1; \
	    $(AR) a atmos.lib $$i/*.o || exit 1; \
	done
	cp atmos/*.joy .
	cp atmos/*.tgi .
	if [ -d atmos/extra ]; then \
	    for i in atmos/extra/*.o; do \
	       	cp $$i atmos-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# C16, C116

.PHONY:	c16lib
c16lib:
	for i in runtime c16 cbm common conio dbg em joystick mouse tgi zlib; do \
	    $(MAKE) SYS=c16 -C $$i || exit 1; \
	    $(AR) a c16.lib $$i/*.o || exit 1; \
	done
	cp c16/*.joy .
	cp c16/*.emd .
	if [ -d c16/extra ]; then \
	    for i in c16/extra/*.o; do \
	       	cp $$i c16-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# C64

.PHONY:	c64lib
c64lib:
	for i in runtime c64 cbm common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=c64 -C $$i || exit 1; \
	    $(AR) a c64.lib $$i/*.o || exit 1; \
	done
	cp c64/*.emd .
	cp c64/*.joy .
	cp c64/c64-1351.mou .
	cp c64/c64-joymouse.mou c64-joy.mou
	cp c64/c64-potmouse.mou c64-pot.mou
	cp c64/*.ser .
	cp c64/c64-320-200-2.tgi c64-hi.tgi
	if [ -d c64/extra ]; then \
	    for i in c64/extra/*.o; do \
	       	cp $$i c64-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# C128

.PHONY:	c128lib
c128lib:
	for i in runtime c128 cbm common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=c128 -C $$i || exit 1; \
	    $(AR) a c128.lib $$i/*.o || exit 1; \
	done
	cp c128/*.emd .
	cp c128/*.joy .
	cp c128/c128-1351.mou .
	cp c128/c128-joymouse.mou c128-joy.mou
	cp c128/c128-potmouse.mou c128-pot.mou
	cp c128/*.ser .
	cp c128/c128-640-200-2.tgi c128-vdc.tgi
	cp c128/c128-640-480-2.tgi c128-vdc2.tgi
	if [ -d cbm/extra ]; then \
	    for i in cbm/extra/*.o; do \
	       	cp $$i cbm-`basename $$i` || exit 1; \
	    done \
	fi
	if [ -d c128/extra ]; then \
	    for i in c128/extra/*.o; do \
	       	cp $$i c128-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Commodore P500 / CBM 5x0

.PHONY:	cbm510lib
cbm510lib:
	for i in runtime cbm510 cbm common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=cbm510 -C $$i || exit 1; \
	    $(AR) a cbm510.lib $$i/*.o || exit 1; \
	done
	cp cbm510/*.emd .
	cp cbm510/cbm510-stdjoy.joy cbm510-std.joy
	cp cbm510/cbm510-stdser.ser cbm510-std.ser
	if [ -d cbm510/extra ]; then \
	    for i in cbm510/extra/*.o; do \
	       	cp $$i cbm510-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# PET-II series

.PHONY:	cbm610lib
cbm610lib:
	for i in runtime cbm610 cbm common conio dbg em joystick serial tgi zlib; do \
	    $(MAKE) SYS=cbm610 -C $$i || exit 1; \
	    $(AR) a cbm610.lib $$i/*.o || exit 1; \
	done
	cp cbm610/*.emd .
	cp cbm610/cbm610-stdser.ser cbm610-std.ser
	if [ -d cbm610/extra ]; then \
	    for i in cbm610/extra/*.o; do \
	       	cp $$i cbm610-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# GEOS on the C64/128

.PHONY:	geoslib
geoslib:
	CC=../$(CC) \
	AS=../$(AS) \
	AR=../$(AR) \
	LD=../$(LD) \
	AFLAGS="-t geos --forget-inc-paths -I../../../asminc" \
	CFLAGS="-Osir -g -T -t geos --forget-inc-paths -I. -I../../../include" \
	$(MAKE) -C geos
	for i in em joystick tgi conio common runtime zlib; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t geos --forget-inc-paths -I../../asminc" \
	    CFLAGS="-Osir -g -T -t geos --forget-inc-paths -I. -I../../include" \
	    $(MAKE) SYS=geos -C $$i || exit 1; \
	    for objfile in $$i/*.o; do \
	        if [ -f geos/$$objfile ]; then \
	      	    $(AR) a geos.lib geos/$$objfile || exit 1; \
	      	else \
	      	    $(AR) a geos.lib $$objfile || exit 1; \
	      	fi; \
	    done \
	done
	cp geos/devel/*.emd .
	cp geos/devel/*.joy .
	cp geos/devel/*.tgi .
	if [ -d geos/extra ]; then \
	    for i in geos/extra/*.o; do \
	       	cp $$i geos-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Lynx

.PHONY:	lynxlib
lynxlib:
	for i in runtime lynx common conio em joystick serial tgi zlib; do \
	    $(MAKE) SYS=lynx -C $$i || exit 1; \
	    $(AR) a lynx.lib $$i/*.o || exit 1; \
	done
	cp lynx/*.joy .
	cp lynx/*.ser .
	cp lynx/*.tgi .
	if [ -d lynx/extra ]; then \
	    for i in lynx/extra/*.o; do \
	       	cp $$i lynx-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# NES

.PHONY: neslib
neslib:
	for i in runtime nes common conio em joystick tgi zlib; do \
	    $(MAKE) SYS=nes -C $$i || exit 1; \
	    $(AR) a nes.lib $$i/*.o || exit 1; \
	done
	cp nes/*.joy .
	if [ -d nes/extra ]; then \
	    for i in nes/extra/*.o; do \
	       	cp $$i nes-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# CBM PET machines

.PHONY:	petlib
petlib:
	for i in runtime pet cbm common conio dbg em joystick tgi zlib; do \
	    $(MAKE) SYS=pet -C $$i || exit 1; \
	    $(AR) a pet.lib $$i/*.o || exit 1; \
	done
	cp pet/*.joy .
	if [ -d pet/extra ]; then \
	    for i in pet/extra/*.o; do \
	       	cp $$i pet-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Commodore Plus/4

.PHONY:	plus4lib
plus4lib:
	for i in runtime plus4 cbm common conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=plus4 -C $$i || exit 1; \
	    $(AR) a plus4.lib $$i/*.o || exit 1; \
	done
	cp plus4/*.joy .
	cp plus4/*.ser .
	if [ -d plus4/extra ]; then \
	    for i in plus4/extra/*.o; do \
	       	cp $$i plus4-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Supervision

.PHONY: supervisionlib
supervisionlib:
	for i in runtime supervision common; do \
	    $(MAKE) SYS=supervision -C $$i || exit 1; \
	    $(AR) a supervision.lib $$i/*.o || exit 1; \
	done
	if [ -d supervision/extra ]; then \
	    for i in supervision/extra/*.o; do \
	       	cp $$i supervision-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Vic20

.PHONY:	vic20lib
vic20lib:
	for i in runtime vic20 cbm common conio dbg em joystick mouse tgi zlib; do \
	    $(MAKE) SYS=vic20 -C $$i || exit 1; \
	    $(AR) a vic20.lib $$i/*.o || exit 1; \
	done
	cp vic20/*.joy .
	if [ -d vic20/extra ]; then \
	    for i in vic20/extra/*.o; do \
	       	cp $$i vic20-`basename $$i` || exit 1; \
	    done \
	fi

#-----------------------------------------------------------------------------
# Dummy targets

.PHONY: mostly-clean
mostly-clean:
	@for i in $(SHAREDDIRS); do \
	   $(MAKE) --no-print-directory -C $$i clean || exit 1; \
	done

.PHONY: clean
clean:	mostly-clean
	@for i in $(ALLTARGETS); do \
	   $(MAKE) -C $$i $@ || exit 1; \
	done

.PHONY: zap
zap:
	@for i in $(SHAREDDIRS) $(ALLTARGETS); do \
	   $(MAKE) -C $$i $@ || exit 1; \
	done
	@$(RM) *.lib *.o *.emd *.joy *.mou *.ser *.tgi
