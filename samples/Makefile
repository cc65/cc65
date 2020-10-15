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

  EMD := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/emd/,$(notdir $(filter %.emd,$(EMD))))
  MOU := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/mou/,$(notdir $(filter %.mou,$(MOU))))
  TGI := $(addprefix $(SUBST_TARGET_PATH)/$(SYS)/drv/tgi/,$(notdir $(filter %.tgi,$(TGI))))

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

.PHONY: all mostlyclean clean install zip samples disk

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
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
else
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -t $(SYS) -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# Lists of subdirectories

# disasm depends on cpp
DIRLIST = tutorial geos

# --------------------------------------------------------------------------
# Lists of executables

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
        
EXELIST_supervision = \
        supervisionhello

# Unlisted targets will try to build everything.
# That lets us learn what they cannot build, and what settings
# we need to use for programs that can be built and run.
ifndef EXELIST_$(SYS)
EXELIST_$(SYS) := ${patsubst %.c,%,$(wildcard *.c)}
endif

define SUBDIR_recipe

@$(MAKE) -C $(dir) --no-print-directory $@

endef # SUBDIR_recipe

# --------------------------------------------------------------------------
# Rules to make the binaries and the disk

samples: $(EXELIST_$(SYS))
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))

disk: $(DISK_$(SYS))

all:

# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.

multdemo: multidemo.o
	$(LD) $(LDFLAGS) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

ovrldemo: overlaydemo.o
	$(LD) $(LDFLAGS) -o $@ -C $(SYS)-overlay.cfg -m $@.map $^ $(SYS).lib

OVERLAYLIST := $(foreach I,1 2 3,multdemo.$I ovrldemo.$I)

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
# Rule to make a CBM disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

define D64_WRITE_PRG_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),p >$(NULLDEV)

endef # D64_WRITE_PRG_recipe

define D64_WRITE_SEQ_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),s >$(NULLDEV)

endef # D64_WRITE_SEQ_recipe

samples.d64: samples
	@$(C1541) -format samples,AA d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(OVERLAYLIST),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_SEQ_recipe))

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
	$(foreach file,$(EMD) $(MOU) $(TGI),$(DSK_WRITE_REL_recipe))

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
	@$(foreach file,$(EMD) $(MOU) $(TGI),$(ATR_WRITE_recipe))
	$(DIR2ATR) -d -b MyDos4534 3200 $@ atr
	@$(RMDIR) atr

# --------------------------------------------------------------------------
# Installation rules

INSTALL = install
samplesdir = $(PREFIX)/share/cc65/samples

install:
	$(if $(PREFIX),,$(error variable "PREFIX" must be set))
	$(INSTALL) -d $(DESTDIR)$(samplesdir)
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/geos
	$(INSTALL) -d $(DESTDIR)$(samplesdir)/tutorial
	$(INSTALL) -m0644 *.* $(DESTDIR)$(samplesdir)
	$(INSTALL) -m0644 readme.txt $(DESTDIR)$(samplesdir)
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
	@$(DEL) *.lbl *.map *.o *.s 2>$(NULLDEV)

clean: mostlyclean
	@$(DEL) $(EXELIST_$(SYS)) $(DISK_$(SYS)) 2>$(NULLDEV)
	@$(DEL) multdemo.? ovrldemo.? 2>$(NULLDEV)
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))
