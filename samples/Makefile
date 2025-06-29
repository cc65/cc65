# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= c64

# Just the usual way to define a variable
# containing a single space character.
SPACE :=
SPACE +=

# Just the usual way to find out if we're
# using cmd.exe to execute make rules.
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

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
    PQ = "QUIET=1"
    PD = --no-print-directory
endif

ifneq ($(filter disk samples.%,$(MAKECMDGOALS)),)
  ifdef CC65_HOME
    TARGET_PATH = $(CC65_HOME)/target
  else
    TARGET_PATH := $(if $(wildcard ../target),../target,$(shell $(CL) --print-target-path))
  endif

  # If TARGET_PATH contains spaces then it is presumed to contain escaped spaces. GNU make
  # has very limited support for paths containing spaces. $(wildcard) is the only function
  # that is aware of escaped spaces. However, $(wildcard) never returns paths with escaped
  # spaces !!! So if it e.g. finds 4 files in a path with 2 spaces then one ends up with a
  # return value consisting of 12 plain words :-((
  #
  # Fortunately we can work around that behaviour here because we know that the files we
  # are looking for have known extensions. So we can $(filter) the in our example above 12
  # words for file extensions so we come up with 4 path fragments. Then we remove those
  # path fragments with $(notdir) from the file names.
  #
  # So far so good. But here we want to process files from different paths in a single
  # recipe further down below and therefore want to prepend the paths to the files with
  # $(addprefix). However, $(foreach) isn't aware of escaped spaces (only $(wildcard) is).
  # Therefore, we need to replace the spaces with some other character temporarily in order
  # to have $(foreach) generate one invocation per file. We use the character '?' for that
  # purpose here, just because it is known to not be part of file names.
  #
  # Inside the recipe generated per file we then replace the '?' again with a space. As we
  # want to be compatible with cmd.exe for execution we're not using an escaped space but
  # rather double-quote the whole path.
  #
  # Note: The "strange" $(wildcard) further down below just serves the purpose to unescape
  #       spaces for cmd.exe. This could have as well been done with another $(subst).

  SUBST_TARGET_PATH := $(subst \$(SPACE),?,$(TARGET_PATH))

  EMD := $(wildcard $(TARGET_PATH)/$(SYS)/drv/emd/*)
  MOU := $(wildcard $(TARGET_PATH)/$(SYS)/drv/mou/*)
  TGI := $(wildcard $(TARGET_PATH)/$(SYS)/drv/tgi/*)
  JOY := $(wildcard $(TARGET_PATH)/$(SYS)/drv/joy/*)

  EMD := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/emd/,$(notdir $(filter %.emd,$(EMD))))
  MOU := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/mou/,$(notdir $(filter %.mou,$(MOU))))
  TGI := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/tgi/,$(notdir $(filter %.tgi,$(TGI))))
  JOY := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/joy/,$(notdir $(filter %.joy,$(JOY))))

  # This one comes with the VICE emulator.
  # See http://vice-emu.sourceforge.net/
  C1541 ?= c1541

  # For this one, see https://applecommander.github.io/
  AC ?= ac.jar

  # For this one, see https://www.horus.com/~hias/atari/
  DIR2ATR ?= dir2atr
endif

DISK_c64       = samples.d64
DISK_apple2    = samples.dsk
DISK_apple2enh = samples.dsk
DISK_atari     = samples.atr
DISK_atarixl   = samples.atr
DISK_plus4     = samples.d64
DISK_c65       = samples.d81
DISK_mega65    = samples.d81

# --------------------------------------------------------------------------
# System-dependent settings
# For convenience, these groups and lines are sorted alphabetically, first
# by target-machine group, then by mission, then by program and sub-target.

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

.PHONY: samples all mostlyclean clean install zip disk platforms

%: %.c
%: %.s

.c.o:
	$(CC) $(CFLAGS) -Ors --codesize 500 -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	$(AS) $(ASFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
ifeq ($(SYS),vic20)
	$(if $(QUIET),echo $(SYS):$@)
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
else ifeq ($(SYS),plus4)
	$(if $(QUIET),echo $(SYS):$@)
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -C plus4-hires.cfg -m $@.map $^ $(SYS).lib
else
	$(if $(QUIET),echo $(SYS):$@)
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -t $(SYS) -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# Lists of subdirectories

# disasm depends on cpp
DIRLIST = tutorial geos atari2600 atari5200 apple2 gamate lynx supervision sym1 kim1 cbm

# --------------------------------------------------------------------------
# Lists of executables
EXELIST_agat =     \
        ascii      \
        checkversion \
        hello      \
        sieve

EXELIST_apple2 =   \
        ascii      \
        checkversion \
        diodemo    \
        enumdevdir \
        gunzip65   \
        hello      \
        joydemo    \
        mandelbrot \
        mousedemo  \
        multdemo   \
        ovrldemo   \
        sieve      \
        terminal   \
        tinyshell  \
        tgidemo

EXELIST_apple2enh = $(EXELIST_apple2)

EXELIST_atari =    \
        ascii      \
        checkversion \
        gunzip65   \
        hello      \
        mandelbrot \
        mousedemo  \
        multdemo   \
        ovrldemo   \
        sieve      \
        terminal   \
        tinyshell  \
        tgidemo

EXELIST_atarixl = $(EXELIST_atari)

EXELIST_atari2600 = \
        notavailable

EXELIST_atari5200 = \
        notavailable

EXELIST_atari7800 = \
        notavailable

EXELIST_atmos =    \
        ascii      \
        checkversion \
        hello      \
        joydemo    \
        mandelbrot \
        sieve      \
        terminal   \
        tgidemo

EXELIST_bbc = \
        notavailable

EXELIST_c64 =      \
        ascii      \
        checkversion \
        enumdevdir \
        gunzip65   \
        hello      \
        joydemo    \
        mandelbrot \
        mousedemo  \
        multdemo   \
        ovrldemo   \
        sieve      \
        terminal   \
        tinyshell  \
        tgidemo

EXELIST_c65 = \
        ascii \
        checkversion \
        enumdevdir \
        hello \
        joydemo    \
        sieve \
        tinyshell

EXELIST_c128 =     \
        ascii      \
        checkversion \
        enumdevdir \
        gunzip65   \
        hello      \
        joydemo    \
        mandelbrot \
        mousedemo  \
        sieve      \
        terminal   \
        tinyshell  \
        tgidemo

EXELIST_c16 =      \
        ascii      \
        checkversion \
        enumdevdir \
        tinyshell  \
        hello      \
        joydemo

EXELIST_cbm510 =   \
        ascii      \
        checkversion \
        gunzip65   \
        hello      \
        joydemo    \
        mousedemo  \
        terminal   \
        tinyshell  \
        sieve

EXELIST_cbm610 =   \
        ascii      \
        checkversion \
        gunzip65   \
        hello      \
        terminal   \
        tinyshell  \
        sieve

EXELIST_creativision = \
        ascii          \
        hello

EXELIST_cx16 =     \
        ascii      \
        checkversion \
        enumdevdir \
        gunzip65   \
        hello      \
        joydemo    \
        mandelbrot \
        mousedemo  \
        sieve      \
        tinyshell  \
        tgidemo

EXELIST_gamate =   \
        hello

EXELIST_geos-cbm = \
        ascii

EXELIST_geos-apple = \
        ascii

EXELIST_lunix = \
        notavailable

EXELIST_lynx = \
        notavailable

EXELIST_mega65 = \
        ascii \
        checkversion \
        enumdevdir \
        hello \
        joydemo    \
        sieve \
        tinyshell

EXELIST_nes = \
        hello      \
        joydemo

EXELIST_osic1p = \
        notavailable

EXELIST_pce = \
        hello      \
        joydemo

EXELIST_pet =      \
        ascii      \
        checkversion \
        enumdevdir \
        hello      \
        joydemo    \
        tinyshell  \
        sieve

EXELIST_plus4 =    \
        ascii      \
        checkversion \
        enumdevdir \
        gunzip65   \
        hello      \
        joydemo    \
        mandelbrot \
        terminal   \
        tinyshell  \
        sieve      \
        tgidemo

EXELIST_sim6502 =  \
        checkversion \
        gunzip65

EXELIST_sim65c02 = $(EXELIST_sim6502)

EXELIST_rp6502 = \
        notavailable

EXELIST_supervision = \
        notavailable

EXELIST_sym1 =     \
        notavailable

EXELIST_kim1 =     \
        notavailable

EXELIST_telestrat = \
        ascii       \
        checkversion \
        gunzip65    \
        hello       \
        joydemo    \
        mandelbrot  \
        sieve       \
        tgidemo

EXELIST_vic20 =    \
        ascii      \
        checkversion \
        enumdevdir \
        hello      \
        joydemo    \
        mandelbrot \
        sieve      \
        tgidemo

# Unlisted targets will try to build everything.
# That lets us learn what they cannot build, and what settings
# we need to use for programs that can be built and run.
ifndef EXELIST_$(SYS)
EXELIST_$(SYS) := ${patsubst %.c,%,$(wildcard *.c)}
endif

define SUBDIR_recipe

@+$(MAKE) -C $(dir) $(PD) $@ $(PQ)

endef # SUBDIR_recipe

# --------------------------------------------------------------------------
# Rules to make the binaries and the disk

samples: $(EXELIST_$(SYS))
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: generic samples not available for" $(SYS)
endif

disk: $(DISK_$(SYS))

all:

# --------------------------------------------------------------------------
# List of every supported platform
TARGETS :=     \
  agat         \
  apple2       \
  apple2enh    \
  atari        \
  atarixl      \
  atari2600    \
  atari5200    \
  atari7800    \
  atmos        \
  bbc          \
  c128         \
  c16          \
  c64          \
  c65          \
  cbm510       \
  cbm610       \
  creativision \
  cx16         \
  gamate       \
  geos-apple   \
  geos-cbm     \
  kim1         \
  lunix        \
  lynx         \
  mega65       \
  nes          \
  osic1p       \
  pce          \
  pet          \
  plus4        \
  rp6502       \
  sim6502      \
  sim65c02     \
  supervision  \
  sym1         \
  telestrat    \
  vic20


# --------------------------------------------------------------------------
# Rule to make the binaries for every platform

define TARGETDIR_recipe

@+$(MAKE) -C $(dir) $(PD) $(PQ)

endef # TARGETDIR_recipe

define TARGET_recipe

@echo making samples for: $(T)
@$(MAKE) -j2 SYS:=$(T) $(PQ)
@$(MAKE) $(PD) clean SYS:=$(T) $(PQ)

endef # TARGET_recipe

platforms:
	$(foreach T,$(TARGETS),$(TARGET_recipe))
	$(foreach dir,$(DIRLIST),$(TARGETDIR_recipe))

# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.

multdemo: multidemo.o
	$(LD) $(LDFLAGS) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

ovrldemo: overlaydemo.o
	$(LD) $(LDFLAGS) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

OVERLAYLIST :=

ifneq ($(filter ovrldemo,$(EXELIST_$(SYS))),)
OVERLAYLIST += $(foreach I,1 2 3,ovrldemo.$I)
endif

ifneq ($(filter multdemo,$(EXELIST_$(SYS))),)
OVERLAYLIST += $(foreach I,1 2 3,multdemo.$I)
endif

# --------------------------------------------------------------------------
# TGI programs on the VIC-20 need a special ld65 configuration file.

ifeq ($(SYS),vic20)
mandelbrot.o: override CFLAGS += -D DYN_DRV=0
mandelbrot: mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -C vic20-tgi.cfg -m $@.map $^ $(SYS).lib

# tgidemo needs at least 16K of RAM expansion.
tgidemo.o: override CFLAGS += -D DYN_DRV=0
tgidemo: tgidemo.o
	$(LD) -D __HIMEM__=0x6000 $(LDFLAGS) -o $@ -C vic20-tgi.cfg -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# some programs link against getsp.o

getsp.o: getsp.s
	$(AS) $(ASFLAGS) -t $(SYS) $<

ifneq ($(SYS),vic20)
tinyshell: tinyshell.o getsp.o
	$(LD) $(LDFLAGS) -t $(SYS) -o $@ $^ $(SYS).lib
endif

# some programs need more memory on the vic20
ifeq ($(SYS),vic20)
tinyshell:  tinyshell.o getsp.o
	$(LD) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

define D64_WRITE_PRG_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),p >$(NULLDEV)

endef # D64_WRITE_PRG_recipe

define D64_WRITE_SEQ_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),s >$(NULLDEV)

endef # D64_WRITE_SEQ_recipe

samples.d64: samples
	@$(C1541) -format "samples,00" d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(OVERLAYLIST),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(EMD) $(MOU) $(JOY) $(TGI),$(D64_WRITE_SEQ_recipe))

samples.d81: samples
	@$(C1541) -format "samples,00" d81 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(OVERLAYLIST),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(EMD) $(MOU) $(JOY) $(TGI),$(D64_WRITE_SEQ_recipe))

# --------------------------------------------------------------------------
# Rule to make an Apple II disk with all samples. Needs the AppleCommander
# program, available at https://applecommander.github.io/, and a template disk
# named 'prodos.dsk'.

define DSK_WRITE_BIN_recipe

$(if $(findstring BF00,$(LDFLAGS_$(notdir $(file))_$(SYS))), \
  java -jar $(AC) -p $@ $(notdir $(file)).system sys <"$(wildcard $(TARGET_PATH)/$(SYS)/util/loader.system)")
java -jar $(AC) -as $@ $(notdir $(file)) <"$(file)"

endef # DSK_WRITE_BIN_recipe

define DSK_WRITE_REL_recipe

java -jar $(AC) -p $@ $(notdir $(file)) rel 0 <"$(subst ?,$(SPACE),$(file))"

endef # DSK_WRITE_REL_recipe

samples.dsk: samples
	cp prodos.dsk $@
	$(foreach file,$(EXELIST_$(SYS)),$(DSK_WRITE_BIN_recipe))
	$(foreach file,$(OVERLAYLIST),$(DSK_WRITE_REL_recipe))
	$(foreach file,$(EMD) $(MOU) $(JOY) $(TGI),$(DSK_WRITE_REL_recipe))

# --------------------------------------------------------------------------
# Rule to make an Atari disk with all samples. Needs the dir2atr program
# available at http://www.horus.com/~hias/atari/ and the MyDos4534 variant
# of dos.sys and dup.sys.

define ATR_WRITE_recipe

cp "$(subst ?,$(SPACE),$(file))" atr/$(notdir $(file))

endef # ATR_WRITE_recipe

samples.atr: samples
	@mkdir atr
	cp "dos.sys" atr/dos.sys
	cp "dup.sys" atr/dup.sys
	@$(foreach file,$(EXELIST_$(SYS)),$(ATR_WRITE_recipe))
	@$(foreach file,$(OVERLAYLIST),$(ATR_WRITE_recipe))
	@$(foreach file,$(EMD) $(MOU) $(JOY) $(TGI),$(ATR_WRITE_recipe))
	$(DIR2ATR) -d -b MyDos4534 3200 $@ atr
	@$(RMDIR) atr

# --------------------------------------------------------------------------
# Installation rules

INSTALL = install
samplesdir = $(PREFIX)/share/cc65/samples

install:
	$(INSTALL) -d $(DESTDIR)$(samplesdir)
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/tutorial
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/atari2600
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/atari5200
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/apple2
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/cbm
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/gamate
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/supervision
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/disasm
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/kim1
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/lynx
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/sim65
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/sym1
	$(INSTALL) -m0644 *.* $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 readme.txt $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 Makefile $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 geos/*.* $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -m0644 tutorial/*.* $(DESTDIR)$(samplesdir)/tutorial
	$(INSTALL) -m0644 atari2600/*.* $(DESTDIR)$(samplesdir)/atari2600
	$(INSTALL) -m0644 atari5200/*.* $(DESTDIR)$(samplesdir)/atari5200
	$(INSTALL) -m0644 apple2/*.* $(DESTDIR)$(samplesdir)/apple2
	$(INSTALL) -m0644 cbm/*.* $(DESTDIR)$(samplesdir)/cbm
	$(INSTALL) -m0644 gamate/*.* $(DESTDIR)$(samplesdir)/gamate
	$(INSTALL) -m0644 supervision/*.* $(DESTDIR)$(samplesdir)/supervision
	$(INSTALL) -m0644 disasm/*.* $(DESTDIR)$(samplesdir)/disasm
	$(INSTALL) -m0644 kim1/*.* $(DESTDIR)$(samplesdir)/kim1
	$(INSTALL) -m0644 lynx/*.* $(DESTDIR)$(samplesdir)/lynx
	$(INSTALL) -m0644 sim65/*.* $(DESTDIR)$(samplesdir)/sim65
	$(INSTALL) -m0644 sym1/*.* $(DESTDIR)$(samplesdir)/sym1

# --------------------------------------------------------------------------
# Packaging rules

zip:
	@cd .. && zip -r cc65 samples/

# --------------------------------------------------------------------------
# Clean-up rules

mostlyclean:
	@$(DEL) *.lbl *.map *.o 2>$(NULLDEV)
# we cant use .s since we have asm files in the directory that we want to keep
	@$(DEL) ${patsubst %.c,%.s,$(wildcard *.c)} 2>$(NULLDEV)

clean: mostlyclean
	@$(DEL) $(EXELIST_$(SYS)) $(DISK_$(SYS)) 2>$(NULLDEV)
	@$(DEL) multdemo.? ovrldemo.? 2>$(NULLDEV)
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))
