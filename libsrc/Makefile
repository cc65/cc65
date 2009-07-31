#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .obj .s .c

# Defines for executables. AR is used within this makefile, the others are
# passed to the submakes and contain paths relative to the subdirectories
# handled by the make subprocesses.
AR = ../src/ar65/ar65
AS = ../../src/ca65/ca65
CC = ../../src/cc65/cc65
LD = ../../src/ld65/ld65

# List of all targets
ALLTARGETS =   	apple2 		\
		apple2enh	\
       	       	atari  		\
       	       	atmos  		\
                c16    		\
       	       	c128   		\
       	       	c64    		\
       	       	cbm510 		\
       	       	cbm610 		\
       	       	geos   		\
		lynx		\
                nes     	\
       	       	pet    		\
       	       	plus4   	\
		supervision	\
		vic20

#-----------------------------------------------------------------------------

.PHONY:	all
all:
	for tgt in $(ALLTARGETS); do		\
	    $(MAKE) clean "$$tgt"lib || exit 1;	\
	done

#-----------------------------------------------------------------------------
# Apple ][

apple2lib:
	for i in apple2 common runtime conio dbg em joystick mouse serial tgi zlib; do \
       	    $(MAKE) SYS=apple2 -C $$i || exit 1; \
	    $(AR) a apple2.lib $$i/*.o;\
	done
	cp apple2/apple2-auxmem.emd a2.auxmem.emd
	cp apple2/apple2-stdjoy.joy a2.stdjoy.joy
	cp apple2/apple2-stdmou.mou a2.stdmou.mou
	cp apple2/apple2-ssc.ser a2.ssc.ser
	cp apple2/apple2-280-192-8.tgi a2.hi.tgi
	cp apple2/apple2-40-48-16.tgi a2.lo.tgi

#-----------------------------------------------------------------------------
# Apple //e

apple2enhlib:
	for i in apple2enh common runtime conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=apple2enh -C $$i || exit 1; \
	    $(AR) a apple2enh.lib $$i/*.o;\
	done
	cp apple2enh/apple2-auxmem.emd a2e.auxmem.emd
	cp apple2enh/apple2-stdjoy.joy a2e.stdjoy.joy
	cp apple2enh/apple2-stdmou.mou a2e.stdmou.mou
	cp apple2enh/apple2-ssc.ser a2e.ssc.ser
	cp apple2enh/apple2-280-192-8.tgi a2e.hi.tgi
	cp apple2enh/apple2-40-48-16.tgi a2e.lo.tgi

#-----------------------------------------------------------------------------
# Atari

atarilib:
	for i in atari common runtime conio dbg em joystick tgi zlib; do \
       	    $(MAKE) SYS=atari -C $$i || exit 1; \
	    $(AR) a atari.lib $$i/*.o;\
	done
	cp atari/atari-stdjoy.joy ataristd.joy
	cp atari/atari-multijoy.joy atarimj8.joy

#-----------------------------------------------------------------------------
# Oric Atmos

atmoslib:
	for i in atmos common conio runtime em joystick tgi zlib; do \
       	    $(MAKE) SYS=atmos -C $$i || exit 1; \
	    $(AR) a atmos.lib $$i/*.o;\
	done
	cp atmos/*.tgi .

#-----------------------------------------------------------------------------
# C16, C116

c16lib:
	for i in c16 cbm common runtime conio dbg em joystick tgi zlib; do \
       	    $(MAKE) SYS=c16 -C $$i || exit 1; \
	    $(AR) a c16.lib $$i/*.o;\
	done
	cp c16/*.joy .
	cp c16/*.emd .

#-----------------------------------------------------------------------------
# C64

c64lib:
	for i in c64 cbm common runtime conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=c64 -C $$i || exit 1; \
	    $(AR) a c64.lib $$i/*.o;\
	done
	cp c64/*.emd .
	cp c64/*.joy .
	cp c64/c64-1351.mou .
	cp c64/c64-joymouse.mou c64-joy.mou
	cp c64/c64-potmouse.mou c64-pot.mou
	cp c64/*.ser .
	cp c64/c64-320-200-2.tgi c64-hi.tgi

#-----------------------------------------------------------------------------
# C128

c128lib:
	for i in c128 cbm common runtime conio dbg em joystick mouse serial tgi zlib; do \
	    $(MAKE) SYS=c128 -C $$i || exit 1; \
	    $(AR) a c128.lib $$i/*.o;\
	done
	cp c128/*.emd .
	cp c128/*.joy .
	cp c128/c128-1351.mou .
	cp c128/c128-joymouse.mou c128-joy.mou
	cp c128/c128-potmouse.mou c128-pot.mou
	cp c128/*.ser .
	cp c128/c128-640-200-2.tgi c128-vdc.tgi
	cp c128/c128-640-480-2.tgi c128-vdc2.tgi

#-----------------------------------------------------------------------------
# Commdore P500 / CBM 5x0

cbm510lib:
	for i in cbm510 cbm common runtime conio dbg em joystick serial tgi zlib; do \
	    $(MAKE) SYS=cbm510 -C $$i || exit 1; \
	    $(AR) a cbm510.lib $$i/*.o;\
	done
	cp cbm510/*.emd .
	cp cbm510/cbm510-stdjoy.joy cbm510-std.joy
	cp cbm510/cbm510-stdser.ser cbm510-std.ser

#-----------------------------------------------------------------------------
# PET-II series

cbm610lib:
	for i in cbm610 cbm common runtime conio dbg em joystick serial tgi zlib; do \
	    $(MAKE) SYS=cbm610 -C $$i || exit 1; \
	    $(AR) a cbm610.lib $$i/*.o;\
	done
	cp cbm610/*.emd .
	cp cbm610/cbm610-stdser.ser cbm610-std.ser

#-----------------------------------------------------------------------------
# GEOS on the C64/128

geoslib:
	CC=../$(CC) \
	AS=../$(AS) \
	AR=../$(AR) \
       	LD=../$(LD) \
	AFLAGS="-t geos -I../../../asminc" \
	CFLAGS="-Osir -g -T -t geos --forget-inc-paths -I. -I../../../include" \
	$(MAKE) -C geos || exit 1
	for i in em joystick tgi conio common runtime zlib; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AR=$(AR) \
	    AFLAGS="-t geos -I../../asminc" \
	    CFLAGS="-Osir -g -T -t geos --forget-inc-paths -I. -I../../include" \
	    $(MAKE) SYS=geos -C $$i || exit 1; \
	    for objfile in $$i/*.o; do \
	        if [ -f geos/$$objfile ]; then \
	      	    $(AR) a geos.lib geos/$$objfile; \
	      	else \
	      	    $(AR) a geos.lib $$objfile; \
	      	fi; \
	    done \
	done
	cp geos/devel/*.emd .
	cp geos/devel/*.joy .
	cp geos/devel/geos-tgi.tgi geos-tgi.tgi

#-----------------------------------------------------------------------------
# Lynx

lynxlib:
	for i in lynx common conio runtime em joystick tgi zlib; do \
	    $(MAKE) SYS=lynx -C $$i || exit 1; \
	    $(AR) a lynx.lib $$i/*.o;\
	done
	cp lynx/*.joy .
	cp lynx/*.tgi .

#-----------------------------------------------------------------------------
# NES

neslib:
	for i in nes common runtime conio em joystick tgi zlib; do \
	    $(MAKE) SYS=nes -C $$i || exit 1; \
	    $(AR) a nes.lib $$i/*.o;\
	done
	cp nes/*.joy .

#-----------------------------------------------------------------------------
# CBM PET machines

petlib:
	for i in pet cbm common runtime conio dbg em joystick tgi zlib; do \
	    $(MAKE) SYS=pet -C $$i || exit 1; \
	    $(AR) a pet.lib $$i/*.o;\
	done
	cp pet/*.joy .

#-----------------------------------------------------------------------------
# Commodore Plus/4

plus4lib:
	for i in plus4 cbm common runtime conio dbg em joystick serial tgi zlib; do \
	    $(MAKE) SYS=plus4 -C $$i || exit 1; \
	    $(AR) a plus4.lib $$i/*.o;\
	done
	cp plus4/*.joy .
	cp plus4/*.ser .

#-----------------------------------------------------------------------------
# Supervision

supervisionlib:
	for i in supervision common runtime; do \
	    $(MAKE) SYS=supervision -C $$i || exit 1; \
	    $(AR) a supervision.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Vic20

vic20lib:
	for i in vic20 cbm common runtime conio dbg em joystick tgi zlib; do \
	    $(MAKE) SYS=vic20 -C $$i || exit 1; \
	    $(AR) a vic20.lib $$i/*.o;\
	done
	cp vic20/*.joy .

#-----------------------------------------------------------------------------
# Dummy targets

.PHONY: clean
clean:
	@for i in cbm common conio dbg em joystick mouse runtime serial tgi zlib $(ALLTARGETS); do\
       	   $(MAKE) -C $$i clean;			      		   	\
	done

.PHONY: zap
zap:	clean
	@for i in cbm common conio dbg em joystick mouse runtime tgi zlib $(ALLTARGETS); do\
	   $(MAKE) -C $$i zap;     	    	       	      	   		\
	done
	@$(RM) *.o *.emd *.joy *.lib *.tgi *.ser *.mou



