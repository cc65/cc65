#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .obj .s .c

# Defines for executables. The first two are passed to the submakes and are
# relative to the subdirectories, the last one is used directly.
CC = ../../src/cc65/cc65
AS = ../../src/ca65/ca65
AR = ../src/ar65/ar65

# List of all targets
ALLTARGETS =	apple2lib	\
		atarilib 	\
		atmoslib	\
		c64lib	 	\
		c128lib	 	\
		cbm510lib	\
		cbm610lib	\
		geoslib	 	\
		petlib	 	\
		plus4lib

#-----------------------------------------------------------------------------

all:
	for tgt in $(ALLTARGETS); do		\
	    $(MAKE) clean $$tgt || exit 1;	\
	done

#-----------------------------------------------------------------------------
# Apple ][

apple2lib:
	for i in apple2 common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t apple2 -I../../include" \
	    AFLAGS="-t apple2 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv apple2/crt0.o apple2.o
	for i in apple2 common runtime conio dbg; do \
	    $(AR) a apple2.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Atari

atarilib:
	for i in atari common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t atari -I../../include" \
	    AFLAGS="-t atari -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv atari/crt0.o atari.o
	for i in atari common runtime conio dbg; do \
	    $(AR) a atari.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Oric Atmos

atmoslib:
	for i in atmos common runtime; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t atmos -I../../include" \
       	    AFLAGS="-t atmos -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv atmos/crt0.o atmos.o
	for i in atmos common runtime; do \
	    $(AR) a atmos.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# C64

c64lib:
	for i in c64 cbm common runtime conio dbg tgi; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t c64 -I../../include" \
	    AFLAGS="-t c64 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv c64/crt0.o c64.o
	for i in c64 cbm common runtime conio dbg tgi; do \
	    $(AR) a c64.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# C128

c128lib:
	for i in c128 cbm common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t c128 -I../../include" \
	    AFLAGS="-t c128 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv c128/crt0.o c128.o
	for i in c128 cbm common runtime conio dbg; do \
	    $(AR) a c128.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Commdore P500 / CBM 5x0

cbm510lib:
	for i in cbm510 cbm common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t cbm510 -I../../include" \
	    AFLAGS="-t cbm510 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv cbm510/crt0.o cbm510.o
	for i in cbm510 cbm common runtime conio dbg; do \
	    $(AR) a cbm510.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# PET-II series

cbm610lib:
	for i in cbm610 cbm common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t cbm610 -I../../include" \
	    AFLAGS="-t cbm610 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv cbm610/crt0.o cbm610.o
	for i in cbm610 cbm common runtime conio dbg; do \
	    $(AR) a cbm610.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# GEOS on the C64/128

geoslib:
	CC=../$(CC) \
	AS=../$(AS) \
	AR=../$(AR) \
	CFLAGS="-Osir -g -T -t geos -I../../../include" \
	AFLAGS="-t geos -I../../../asminc" \
	$(MAKE) -C geos || exit 1
	for i in common runtime; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    AR=$(AR) \
	    CFLAGS="-Osir -g -T -t geos -I../../include" \
	    AFLAGS="-t geos -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	for i in common runtime; do \
	    for objfile in $$i/*.o; do \
	        if [ -f geos/$$objfile ]; then \
		    $(AR) a geos.lib geos/$$objfile; \
		else \
		    $(AR) a geos.lib $$objfile; \
		fi; \
	    done \
	done

#-----------------------------------------------------------------------------
# CBM PET machines

petlib:
	for i in pet cbm common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t pet -I../../include" \
	    AFLAGS="-t pet -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv pet/crt0.o pet.o
	for i in pet cbm common runtime conio dbg; do \
	    $(AR) a pet.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Commodore C116, C16 and Plus/4

plus4lib:
	for i in plus4 cbm common runtime conio dbg; do \
	    CC=$(CC) \
	    AS=$(AS) \
	    CFLAGS="-Osir -g -T -t plus4 -I../../include" \
	    AFLAGS="-t plus4 -I../../asminc" \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv plus4/crt0.o plus4.o
	for i in plus4 cbm common runtime conio dbg; do \
	    $(AR) a plus4.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Dummy targets

.PHONY: clean
clean:
	@for i in apple2 atari c128 c64 cbm cbm510 cbm610 common conio dbg geos pet plus4 runtime tgi; do 	\
	   $(MAKE) -C $$i clean;     	    		  	   					\
	done

.PHONY: zap
zap:	clean
	@rm -f *.o *.lib






