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
ALLTARGETS =   	apple2	\
		atari 	\
		atmos	\
		vic20	\
                c16  	\
		c64  	\
		c128 	\
		cbm510	\
		cbm610	\
		geos 	\
		pet  	\
		plus4

#-----------------------------------------------------------------------------

.PHONY:	all
all:
	for tgt in $(ALLTARGETS); do		\
	    $(MAKE) clean "$$tgt"lib || exit 1;	\
	done

#-----------------------------------------------------------------------------
# Apple ][

apple2lib:
	for i in apple2 common runtime conio dbg em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t apple2 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t apple2 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a apple2.lib $$i/*.o;\
	done
	mv apple2/crt0.o apple2.o

#-----------------------------------------------------------------------------
# Atari

atarilib:
	for i in atari common runtime conio dbg em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t atari -I../../asminc" \
	    CFLAGS="-Osir -g -T -t atari -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a atari.lib $$i/*.o;\
	done
	mv atari/crt0.o atari.o

#-----------------------------------------------------------------------------
# Oric Atmos

atmoslib:
	for i in atmos common conio runtime em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t atmos -I../../asminc" \
	    CFLAGS="-Osir -g -T -t atmos -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a atmos.lib $$i/*.o;\
	done
	mv atmos/crt0.o atmos.o

#-----------------------------------------------------------------------------
# Vic20

vic20lib:
	for i in vic20 cbm common runtime conio dbg em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t vic20 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t vic20 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a vic20.lib $$i/*.o;\
	done
	mv vic20/crt0.o vic20.o
	cp vic20/*.joy .

#-----------------------------------------------------------------------------
# C16, C116

c16lib:
	for i in c16 cbm common runtime conio dbg em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t c16 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t c16 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a c16.lib $$i/*.o;\
	done
	mv c16/crt0.o c16.o
	cp c16/*.joy .

#-----------------------------------------------------------------------------
# C64

c64lib:
	for i in c64 cbm common runtime conio dbg em joystick serial tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    CFLAGS="-Osir -g -T -t c64 -I../../include" \
	    AFLAGS="-t c64 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a c64.lib $$i/*.o;\
	done
	mv c64/crt0.o c64.o
	cp c64/*.emd .
	cp c64/*.joy .
	cp c64/c64-320-200-2.tgi c64-hi.tgi

#-----------------------------------------------------------------------------
# C128

c128lib:
	for i in c128 cbm common runtime conio dbg em joystick tgi; do \
	    AS=$(AS) \
	    CC=$(CC) \
	    LD=$(LD) \
	    AFLAGS="-t c128 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t c128 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a c128.lib $$i/*.o;\
	done
	mv c128/crt0.o c128.o
	cp c128/*.emd .
	cp c128/*.joy .
	cp c128/c128-640-200-2.tgi c128-vdc.tgi
	cp c128/c128-640-480-2.tgi c128-vdc2.tgi

#-----------------------------------------------------------------------------
# Commdore P500 / CBM 5x0

cbm510lib:
	for i in cbm510 cbm common runtime conio dbg em joystick tgi; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t cbm510 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t cbm510 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a cbm510.lib $$i/*.o;\
	done
	mv cbm510/crt0.o cbm510.o
	cp cbm510/*.emd .
	cp cbm510/*.joy .

#-----------------------------------------------------------------------------
# PET-II series

cbm610lib:
	for i in cbm610 cbm common runtime conio dbg em joystick tgi; do \
     	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t cbm610 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t cbm610 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a cbm610.lib $$i/*.o;\
	done
	mv cbm610/crt0.o cbm610.o
	cp cbm610/*.emd .

#-----------------------------------------------------------------------------
# GEOS on the C64/128

geoslib:
	CC=../$(CC) \
	AS=../$(AS) \
	AR=../$(AR) \
       	LD=../$(LD) \
	AFLAGS="-t geos -I../../../asminc" \
	CFLAGS="-Osir -g -T -t geos -I../../../include" \
	$(MAKE) -C geos || exit 1
	for i in em joystick tgi conio common runtime; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AR=$(AR) \
	    AFLAGS="-t geos -I../../asminc" \
	    CFLAGS="-Osir -g -T -t geos -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
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
# NES

neslib:
	for i in nes common runtime conio em joystick tgi; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t nes -I../../asminc" \
	    CFLAGS="-Osir -g -T -t nes -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a nes.lib $$i/*.o;\
	done
	mv nes/crt0.o nes.o

#-----------------------------------------------------------------------------
# CBM PET machines

petlib:
	for i in pet cbm common runtime conio dbg em joystick tgi; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t pet -I../../asminc" \
	    CFLAGS="-Osir -g -T -t pet -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a pet.lib $$i/*.o;\
	done
	mv pet/crt0.o pet.o

#-----------------------------------------------------------------------------
# Commodore Plus/4

plus4lib:
	for i in plus4 cbm common runtime conio dbg em joystick tgi; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    LD=$(LD) \
	    AFLAGS="-t plus4 -I../../asminc" \
	    CFLAGS="-Osir -g -T -t plus4 -I../../include" \
	    $(MAKE) -C $$i || exit 1; \
	    $(AR) a plus4.lib $$i/*.o;\
	done
	mv plus4/crt0.o plus4.o
	cp plus4/*.joy .

#-----------------------------------------------------------------------------
# Dummy targets

.PHONY: clean
clean:
	@for i in cbm common conio dbg em joystick runtime tgi $(ALLTARGETS); do\
       	   $(MAKE) -C $$i clean;			      			\
	done

.PHONY: zap
zap:	clean
	@for i in cbm common conio dbg em joystick runtime tgi $(ALLTARGETS); do\
	   $(MAKE) -C $$i zap;     	    	       	      	   		\
	done
	@rm -f *.o *.emd *.joy *.lib *.tgi


							      
