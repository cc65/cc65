#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .obj .s .c

# Defines used by the submakes:
export CC = ../../src/cc65/cc65
export AS = ../../src/ca65/ca65

# Define used within this makefile
AR = ../src/ar65/ar65

# List of all targets
ALLTARGETS =	apple2lib	\
		atarilib	\
		c64lib		\
		c128lib		\
		cbm610lib	\
		geoslib		\
		petlib		\
		plus4lib

#-----------------------------------------------------------------------------

all:
	for tgt in $(ALLTARGETS); do		\
       	    $(MAKE) clean $$tgt || exit 1;	\
	done

#-----------------------------------------------------------------------------
# Apple ][

apple2lib:
	export CFLAGS="-Osir -g -t apple2 -I../../include";\
	export AFLAGS="-t apple2";\
	for i in apple2 common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv apple2/crt0.o apple2.o
	for i in apple2 common runtime conio dbg; do \
	    $(AR) a apple2.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Atari

atarilib:
	export CFLAGS="-Osir -g -t atari -I../../include";\
       	export AFLAGS="-t atari";\
	for i in atari common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv atari/crt0.o atari.o
	for i in atari common runtime conio dbg; do \
	    $(AR) a atari.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# C64

c64lib:
	export CFLAGS="-Osir -g -t c64 -I../../include";\
       	export AFLAGS="-t c64";\
	for i in c64 cbm common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv c64/crt0.o c64.o
	for i in c64 cbm common runtime conio dbg; do \
	    $(AR) a c64.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# C128

c128lib:
	export CFLAGS="-Osir -g -t c128 -I../../include";\
       	export AFLAGS="-t c128";\
	for i in c128 cbm common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv c128/crt0.o c128.o
	for i in c128 cbm common runtime conio dbg; do \
	    $(AR) a c128.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# PET-II series

cbm610lib:
	export CFLAGS="-Osir -g -t cbm610 -I../../include";\
       	export AFLAGS="-t cbm610";\
	for i in cbm610 cbm common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv cbm610/crt0.o cbm610.o
	for i in cbm610 cbm common runtime conio dbg; do \
	    $(AR) a cbm610.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# GEOS on the C64/128

geoslib:
	export CFLAGS="-Osir -g -t geos -I../../include";\
       	export AFLAGS="-t geos";\
       	for i in geos common runtime; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	for i in common runtime; do \
	    $(AR) a geos.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# CBM PET machines

petlib:
	export CFLAGS="-Osir -g -t pet -I../../include";\
       	export AFLAGS="-t pet";\
	for i in pet cbm common runtime conio dbg; do \
	    $(MAKE) -C $$i || exit 1; \
	done
	mv pet/crt0.o pet.o
	for i in pet cbm common runtime conio dbg; do \
	    $(AR) a pet.lib $$i/*.o;\
	done

#-----------------------------------------------------------------------------
# Commodore C116, C16 and Plus/4

plus4lib:
	export CFLAGS="-Osir -g -t plus4 -I../../include";\
       	export AFLAGS="-t plus4";\
	for i in plus4 cbm common runtime conio dbg; do \
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
	@for i in apple2 atari c128 c64 cbm cbm610 common conio dbg geos pet plus4 runtime; do 	\
       	   $(MAKE) -C $$i clean;     	    		  	   					\
	done

.PHONY: zap
zap:	clean
	@rm -f *.lib




			       
