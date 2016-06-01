#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS = c64

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL = -del /f
else
  NULLDEV = /dev/null
  DEL = $(RM)
endif

# Determine the path to the executables and libraries. If the samples
# directory is part of a complete source tree, use the stuff from that
# source tree; otherwise, use the "install" directories.
ifeq "$(wildcard ../src)" ""
# No source tree
installdir = /usr/lib/cc65
ifneq "$(wildcard /usr/local/lib/cc65)" ""
installdir = /usr/local/lib/cc65
endif
ifneq "$(wildcard /opt/local/share/cc65)" ""
installdir = /opt/local/share/cc65
endif
ifdef CC65_HOME
installdir = $(CC65_HOME)
endif

MOUS := $(wildcard $(installdir)/target/$(SYS)/drv/mou/$(SYS)*.mou)
TGI  := $(wildcard $(installdir)/target/$(SYS)/drv/tgi/$(SYS)*.tgi)
CLIB = --lib $(SYS).lib
CL   = cl65
CC   = cc65
AS   = ca65
LD   = ld65

else
# "samples/" is a part of a complete source tree.
export CC65_HOME := $(abspath ..)
MOUS := $(wildcard ../target/$(SYS)/drv/mou/$(SYS)*.mou)
TGI  := $(wildcard ../target/$(SYS)/drv/tgi/$(SYS)*.tgi)
CLIB = ../lib/$(SYS).lib
CL   = ../bin/cl65
CC   = ../bin/cc65
AS   = ../bin/ca65
LD   = ../bin/ld65
endif

# This one comes with VICE
C1541 ?= c1541

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

.PHONY: all mostlyclean clean install zip samples d64

%: %.c
%: %.s

.c.o:
	$(CC) $(CFLAGS) -Oirs --codesize 500 -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	$(AS) $(AFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) -o $@ -t $(SYS) -m $@.map $^ $(CLIB)

# --------------------------------------------------------------------------
# List of executables. This list could be made target-dependent by checking
# $(SYS).

EXELIST = ascii      \
          diodemo    \
          enumdevdir \
          fire       \
          gunzip65   \
          hello      \
          mandelbrot \
          mousetest  \
          multdemo   \
          nachtm     \
          ovrldemo   \
          plasma     \
          sieve      \
          tgidemo

# --------------------------------------------------------------------------
# Rules to make the binaries

all:

samples: $(EXELIST)

# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.

multdemo: multidemo.o
	$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(CLIB)

ovrldemo: overlaydemo.o
	$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(CLIB)

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

d64: samples.d64

define D64_WRITE_recipe

$(C1541) -attach $@ -write $(file) $(notdir $(file)) >$(NULLDEV)

endef # D64_WRITE_recipe

samples.d64: samples
	@$(C1541) -format samples,AA  d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST),$(D64_WRITE_recipe))
	$(foreach file,$(TGI) $(MOUS),$(D64_WRITE_recipe))

# --------------------------------------------------------------------------
# Installation rules

INSTALL = install
samplesdir = $(prefix)/share/cc65

install:
	$(if $(prefix),,$(error variable `prefix' must be set))
	$(INSTALL) -d $(DESTDIR)$(samplesdir)
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -d $$(DESTDIR)$(samplesdir)/tutorial
	$(INSTALL) -m0644 *.* $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 README $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 Makefile $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 geos/*.* $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -m0644 tutorial/*.* $(DESTDIR)$(samplesdir)/tutorial

# --------------------------------------------------------------------------
# Packaging rules

zip:
	@cd .. && zip -r cc65 samples/

# --------------------------------------------------------------------------
# Clean-up rules

mostlyclean:
	@$(DEL) *.map *.o *.s 2>$(NULLDEV)

clean: mostlyclean
	@$(DEL) $(EXELIST) samples.d64 2>$(NULLDEV)
	@$(DEL) multdemo.? ovrldemo.? 2>$(NULLDEV)
