#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Run 'make SYS=<target>' or set a SYS env
# var to build for another target system.
SYS ?= c64

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL = -del /f
  RMDIR = rmdir /s /q
else
  NULLDEV = /dev/null
  DEL = $(RM)
  RMDIR = $(RM) -r
endif

ifdef CC65_HOME
  AS = $(CC65_HOME)/bin/ca65
  CC = $(CC65_HOME)/bin/cc65
  CL = $(CC65_HOME)/bin/cl65
  LD = $(CC65_HOME)/bin/ld65
else
  AS := $(if $(wildcard ../bin/ca65*),../bin/ca65,ca65)
  CC := $(if $(wildcard ../bin/cc65*),../bin/cc65,cc65)
  CL := $(if $(wildcard ../bin/cl65*),../bin/cl65,cl65)
  LD := $(if $(wildcard ../bin/ld65*),../bin/ld65,ld65)
endif

ifneq ($(filter disk samples.%,$(MAKECMDGOALS)),)
  TARGET_PATH := $(shell $(CL) --print-target-path)

  EMD := $(wildcard $(TARGET_PATH)/$(SYS)/drv/emd/*)
  MOU := $(wildcard $(TARGET_PATH)/$(SYS)/drv/mou/*)
  TGI := $(wildcard $(TARGET_PATH)/$(SYS)/drv/tgi/*)

  # This one comes with VICE
  C1541 ?= c1541

  # For this one see https://applecommander.github.io/
  AC ?= ac.jar

  # For this one see http://www.horus.com/~hias/atari/
  DIR2ATR ?= dir2atr

  DISK_c64       = samples.d64
  DISK_apple2    = samples.dsk
  DISK_apple2enh = samples.dsk
  DISK_atari     = samples.atr
  DISK_atarixl   = samples.atr
endif

# --------------------------------------------------------------------------
# System-dependent settings

# The Apple machines need the start address adjusted when using TGI
LDFLAGS_mandelbrot_apple2    = --start-addr 0x4000
LDFLAGS_mandelbrot_apple2enh = --start-addr 0x4000
LDFLAGS_tgidemo_apple2       = --start-addr 0x4000
LDFLAGS_tgidemo_apple2enh    = --start-addr 0x4000

# The Apple ][ needs the start address adjusted for the mousedemo
LDFLAGS_mousedemo_apple2 = --start-addr 0x4000

# The Apple machines need the end address adjusted for large programs
LDFLAGS_gunzip65_apple2    = -D __HIMEM__=0xBF00
LDFLAGS_gunzip65_apple2enh = -D __HIMEM__=0xBF00

# The atari target needs to reserve some memory when using TGI
LDFLAGS_mandelbrot_atari = -D __RESERVED_MEMORY__=0x2000
LDFLAGS_tgidemo_atari    = -D __RESERVED_MEMORY__=0x2000

# The atarixl target needs the start address adjusted when using TGI
LDFLAGS_mandelbrot_atarixl = --start-addr 0x4000
LDFLAGS_tgidemo_atarixl    = --start-addr 0x4000

# --------------------------------------------------------------------------
# Generic rules

.PHONY: all mostlyclean clean install zip samples disk

%: %.c
%: %.s

.c.o:
	$(CC) $(CFLAGS) -Ors --codesize 500 -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	$(AS) $(AFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) -o $@ -t $(SYS) -m $@.map $^ $(SYS).lib

# --------------------------------------------------------------------------
# List of executables

EXELIST_c64 =      \
        ascii      \
        enumdevdir \
        fire       \
        gunzip65   \
        hello      \
        mandelbrot \
        mousedemo  \
        multdemo   \
        nachtm     \
        ovrldemo   \
        plasma     \
        sieve      \
        tgidemo

EXELIST_apple2 =   \
        ascii      \
        diodemo    \
        enumdevdir \
        gunzip65   \
        hello      \
        mandelbrot \
        mousedemo  \
        multdemo   \
        ovrldemo   \
        sieve      \
        tgidemo

EXELIST_apple2enh = $(EXELIST_apple2)

EXELIST_atari =    \
        ascii      \
        gunzip65   \
        hello      \
        mandelbrot \
        mousedemo  \
        multdemo   \
        ovrldemo   \
        sieve      \
        tgidemo

EXELIST_atarixl = $(EXELIST_atari)

EXELIST_atari2600 = \
        atari2600hello

# --------------------------------------------------------------------------
# Rules to make the binaries and the disk

samples: $(EXELIST_$(SYS))

disk: $(DISK_$(SYS))

all:

# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.

multdemo: multidemo.o
	$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

ovrldemo: overlaydemo.o
	$(LD) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

OVERLAYLIST := $(foreach I,1 2 3,multdemo.$I ovrldemo.$I)

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

define D64_WRITE_recipe

$(C1541) -attach $@ -write $(file) $(notdir $(file)) >$(NULLDEV)

endef # D64_WRITE_recipe

samples.d64: samples
	@$(C1541) -format samples,AA  d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_recipe))
	$(foreach file,$(OVERLAYLIST),$(D64_WRITE_recipe))
	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_recipe))

# --------------------------------------------------------------------------
# Rule to make an Apple II disk with all samples. Needs the Apple Commander
# program available at http://applecommander.sourceforge.net/ and a template
# disk named 'prodos.dsk'.

define DSK_WRITE_BIN_recipe

$(if $(findstring BF00,$(LDFLAGS_$(notdir $(file))_$(SYS))), \
  java -jar $(AC) -p $@ $(notdir $(file)).system sys <$(TARGET_PATH)/$(SYS)/util/loader.system)
java -jar $(AC) -cc65 $@ $(notdir $(file)) bin <$(file)

endef # DSK_WRITE_BIN_recipe

define DSK_WRITE_REL_recipe

java -jar $(AC) -p $@ $(notdir $(file)) rel 0 <$(file)

endef # DSK_WRITE_REL_recipe

samples.dsk: samples
	cp prodos.dsk $@
	$(foreach file,$(EXELIST_$(SYS)),$(DSK_WRITE_BIN_recipe))
	$(foreach file,$(OVERLAYLIST),$(DSK_WRITE_REL_recipe))
	$(foreach file,$(EMD) $(MOU) $(TGI),$(DSK_WRITE_REL_recipe))

# --------------------------------------------------------------------------
# Rule to make an Atari disk with all samples. Needs the dir2atr program
# available at http://www.horus.com/~hias/atari/ and the MyDos4534 variant
# of dos.sys and dup.sys.

define ATR_WRITE_recipe

cp $(file) atr/$(notdir $(file))

endef # ATR_WRITE_recipe

samples.atr: samples
	@mkdir atr
	cp dos.sys atr/dos.sys
	cp dup.sys atr/dup.sys
	@$(foreach file,$(EXELIST_$(SYS)),$(ATR_WRITE_recipe))
	@$(foreach file,$(OVERLAYLIST),$(ATR_WRITE_recipe))
	@$(foreach file,$(EMD) $(MOU) $(TGI),$(ATR_WRITE_recipe))
	$(DIR2ATR) -d -b MyDos4534 3200 $@ atr
	@$(RMDIR) atr

# --------------------------------------------------------------------------
# Installation rules

INSTALL = install
samplesdir = $(PREFIX)/share/cc65/samples

install:
	$(if $(PREFIX),,$(error variable `PREFIX' must be set))
	$(INSTALL) -d $(DESTDIR)$(samplesdir)
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/tutorial
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
	@$(DEL) $(EXELIST_$(SYS)) $(DISK_$(SYS)) 2>$(NULLDEV)
	@$(DEL) multdemo.? ovrldemo.? 2>$(NULLDEV)
