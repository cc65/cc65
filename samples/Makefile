#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= c64

# Determine the path to the executables and libraries. If the samples
# directory is part of a complete source tree, use the stuff from that
# source tree; otherwise, use the "install" directories.
ifeq "$(wildcard ../src)" ""
# No source tree
MOUS = /usr/lib/cc65/mou/$(SYS)*.mou
TGI  = /usr/lib/cc65/tgi/$(SYS)*.tgi
ifneq "$(wildcard /usr/local/lib/cc65)" ""
MOUS = /usr/local/lib/cc65/mou/$(SYS)*.mou
TGI  = /usr/local/lib/cc65/tgi/$(SYS)*.tgi
endif
ifdef CC65_HOME
MOUS = $(CC65_HOME)/mou/$(SYS)*.mou
TGI  = $(CC65_HOME)/tgi/$(SYS)*.tgi
endif
CLIB = --lib $(SYS).lib
CL   = cl65
CC   = cc65
AS   = ca65
LD   = ld65

else
# "samples/" is a part of a complete source tree.
export CC65_HOME := $(abspath ..)
MOUS = ../mou/$(SYS)*.mou
TGI  = ../tgi/$(SYS)*.tgi
CLIB = ../lib/$(SYS).lib
CL   = ../bin/cl65
CC   = ../bin/cc65
AS   = ../bin/ca65
LD   = ../bin/ld65
endif

# This one comes with VICE
C1541	= c1541

# --------------------------------------------------------------------------
# System-dependent settings

# The Apple machines need the start address adjusted when using TGI
LDFLAGS_mandelbrot_apple2 = --start-addr 0x4000
LDFLAGS_tgidemo_apple2 = --start-addr 0x4000
LDFLAGS_mandelbrot_apple2enh = --start-addr 0x4000
LDFLAGS_tgidemo_apple2enh = --start-addr 0x4000

# The Apple ][ needs the start address adjusted for the mousetest
LDFLAGS_mousetest_apple2 = --start-addr 0x4000

# The atarixl target needs the start address adjusted when using TGI
LDFLAGS_mandelbrot_atarixl = --start-addr 0x4000
LDFLAGS_tgidemo_atarixl = --start-addr 0x4000

# The atari target needs to reserve some memory when using TGI
LDFLAGS_mandelbrot_atari = -D __RESERVED_MEMORY__=0x2000
LDFLAGS_tgidemo_atari = -D __RESERVED_MEMORY__=0x2000

# --------------------------------------------------------------------------
# Generic rules

%: %.c
%: %.s

.c.o:
	@echo $<
	@$(CC) $(CFLAGS) -Oirs --codesize 500 -T -g -t $(SYS) $<
	@$(AS) $(<:.c=.s)

.s.o:
	@echo $<
	@$(AS) $(AFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
	@$(LD) $(LDFLAGS_$(@F)_$(SYS)) -o $@ -t $(SYS) -m $@.map $^ $(CLIB)

# --------------------------------------------------------------------------
# List of executables. This list could be made target-dependent by checking
# $(SYS).

EXELIST	=	ascii		\
		diodemo		\
		enumdevdir	\
		fire		\
		gunzip65	\
		hello		\
		mandelbrot	\
		mousetest	\
		multdemo	\
		nachtm		\
		ovrldemo	\
		plasma		\
		sieve		\
		tgidemo

# --------------------------------------------------------------------------
# Rules to make the binaries

.PHONY:	all
all:	$(EXELIST)

# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.

multdemo:	multidemo.o
	@$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(CLIB)

ovrldemo:	overlaydemo.o
	@$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(CLIB)

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

.PHONY:	disk
disk:	samples.d64

samples.d64:	all
	@$(C1541) -format samples,AA  d64 $@ > /dev/null
	@for exe in $(EXELIST); do\
	    $(C1541) -attach $@ -write $$exe > /dev/null || exit $$?;\
	done
	@for mod in $(TGI) $(MOUS); do\
	    $(C1541) -attach $@ -write $$mod > /dev/null || exit $$?;\
	done

# --------------------------------------------------------------------------
# Clean-up rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST) samples.d64
	$(RM) multdemo.? ovrldemo.?
