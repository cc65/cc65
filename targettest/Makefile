# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

#
# Makefile for cc65 testcode
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

ifneq ($(filter disk testcode.%,$(MAKECMDGOALS)),)
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

DISK_c64       = testcode.d64
DISK_apple2    = testcode.dsk
DISK_apple2enh = testcode.dsk
DISK_atari     = testcode.atr
DISK_atarixl   = testcode.atr

# --------------------------------------------------------------------------
# System-dependent settings
# For convenience, these groups and lines are sorted alphabetically, first
# by target-machine group, then by mission, then by program and sub-target.

# --------------------------------------------------------------------------
# Generic rules

.PHONY: testcode all mostlyclean clean zip disk platforms

%: %.c
%: %.s

.c.o:
	$(CC) $(CFLAGS) -Ors --codesize 500 -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	$(AS) $(ASFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

LDFLAGS=
ifeq ($(SYS),c64)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),c128)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),c16)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),cbm510)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),cbm610)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),cx16)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),geos-cbm)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),lunix)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),pet)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),pet-overlay)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),plus4)
	LDFLAGS+=-Ln $@.lbl
endif
ifeq ($(SYS),vic20)
	LDFLAGS+=-Ln $@.lbl
endif

ifeq ($(SYS),pce)
	LDFLAGS+=-D__CARTSIZE__=0x8000
endif

.o:
ifeq ($(SYS),vic20)
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
else
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) $(LDFLAGS) -o $@ -t $(SYS) -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# Lists of subdirectories

DIRLIST = accelerator atari cbm gamate pce

# --------------------------------------------------------------------------
# Lists of executables

# omitted: seek
EXELIST_c64 = \
	minimal \
	arg-test \
	clock \
	clock-test \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: seek
EXELIST_c128 = \
	minimal \
	arg-test \
	clock \
	clock-test \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test


# omitted: seek clock clock-test mouse-test ser-test
EXELIST_c16 = \
	minimal \
	arg-test \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: seek ser-test
EXELIST_cbm510 = \
	minimal \
	arg-test \
	clock \
	clock-test \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: joy-test mouse-test seek
EXELIST_cbm610 = \
	minimal \
	arg-test \
	clock \
	clock-test \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	moddiv-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: clock-test em-test mouse-test seek ser-test
EXELIST_pet = \
	minimal \
	arg-test \
	clock \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: clock-test em-test mouse-test seek
EXELIST_plus4 = \
	minimal \
	arg-test \
	clock \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: seek clock-test mouse-test ser-test
EXELIST_vic20 = \
	minimal \
	arg-test \
	clock \
	conio \
	cpeek-test \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: cpeek-test, clock
EXELIST_apple2 = \
	minimal \
	arg-test \
	clock-test \
	conio \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	seek \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

EXELIST_apple2enh = $(EXELIST_apple2)

# omitted: cpeek-test
EXELIST_atari = \
	minimal \
	arg-test \
	clock-test \
	clock \
	conio \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	seek \
	ser-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

EXELIST_atarixl = $(EXELIST_atari)

# omitted: clock-test cpeek-test deb dir-test em-test exec-test1 exec-test2
# joy-test mouse-test rename-test seek ser-test stroserror-test
EXELIST_telestrat = \
	minimal \
	arg-test \
	clock \
	conio \
	cprintf \
	cursor \
	div-test \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	moddiv-test \
	mul-test \
	posixio-test \
	scanf-test \
	strdup-test \
	strnlen \
	strqtok-test \
	uname-test

# omitted: arg-test clock-test clock cpeek-test conio cprintf cursor deb dir-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test
# mouse-test mul-test posixio-test rename-test scanf-test seek ser-test strdup-test
# stroserror-test uname-test
EXELIST_sym1 = \
	minimal \
	div-test \
	moddiv-test \
	strnlen \
	strqtok-test

# omitted: clock-test cpeek-test deb dir-test em-test exec-test1 exec-test2
# fileio-test ft mouse-test posixio-test rename-test seek ser-test
EXELIST_atmos = \
	minimal \
	arg-test \
	clock \
	conio \
	cprintf \
	cursor \
	div-test \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mul-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: arg-test clock-test clock cpeek-test conio cprintf deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test mul-test posixio-test rename-test scanf-test seek ser-test strdup-test
# strnlen stroserror-test strqtok-test uname-test
EXELIST_creativision = \
	minimal \
	cursor

# omitted: cpeek-test seek ser-test
EXELIST_cx16 = \
	minimal \
	arg-test \
	clock-test \
	clock \
	conio \
	cprintf \
	cursor \
	deb \
	dir-test \
	div-test \
	em-test \
	exec-test1 \
	exec-test2 \
	fileio-test \
	ft \
	getopt-test \
	heaptest \
	joy-test \
	moddiv-test \
	mouse-test \
	mul-test \
	posixio-test \
	rename-test \
	scanf-test \
	strdup-test \
	strnlen \
	stroserror-test \
	strqtok-test \
	uname-test

# omitted: arg-test clock-test clock cpeek-test cprintf cursor deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test mul-test posixio-test rename-test scanf-test seek ser-test strdup-test
# strnlen stroserror-test strqtok-test uname-test
EXELIST_nes = \
	minimal \
	conio

# omitted: arg-test clock-test clock cpeek-test cprintf cursor deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test mul-test posixio-test rename-test scanf-test seek ser-test strdup-test
# strnlen stroserror-test strqtok-test uname-test
EXELIST_pce = \
	minimal \
	conio

# omitted: arg-test clock-test clock cpeek-test conio cprintf deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test posixio-test rename-test scanf-test seek ser-test strdup-test strnlen
# stroserror-test strqtok-test uname-test
EXELIST_osic1p = \
	minimal \
	conio \
	cursor \
	mul-test

# omitted: arg-test clock-test clock cpeek-test conio cprintf deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test posixio-test rename-test scanf-test seek ser-test strdup-test strnlen
# stroserror-test strqtok-test uname-test
EXELIST_geos-apple = \
	minimal \
	cursor \
	mul-test

# omitted: arg-test clock-test clock cpeek-test conio cprintf deb dir-test div-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test moddiv-test
# mouse-test posixio-test rename-test scanf-test seek ser-test strdup-test strnlen
# stroserror-test strqtok-test uname-test

EXELIST_geos-cbm = \
	minimal \
	cursor \
	mul-test

# omitted: clock clock-test conio cpeek-test cprintf cursor deb dir-test em-test
# exec-test1 exec-test2 ft heaptest joy-test mouse-test mul-test rename-test seek
# ser-test strdup-test stroserror-test uname-test
EXELIST_sim6502 = \
	minimal \
	arg-test \
	div-test \
	fileio-test \
	getopt-test \
	moddiv-test \
	posixio-test \
	scanf-test \
	strnlen \
	strqtok-test

EXELIST_sim65c02 = $(EXELIST_sim6502)


# omitted: arg-test clock clock-test conio cpeek-test cprintf cursor deb dir-test
# div-test em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest
# joy-test moddiv-test mouse-test mul-test posixio-test rename-test scanf-test
# ser-test seek strdup-test strnlen stroserror-test strqtok-test tinyshell uname-test
EXELIST_atari2600 = \
	minimal

EXELIST_atari5200 = \
	minimal

EXELIST_atari7800 = \
	minimal

EXELIST_gamate = \
	minimal

EXELIST_lynx = \
	minimal

EXELIST_supervision = \
	minimal

# none of the testcode can work on the bbc (no library support)
EXELIST_bbc = \
	notavailable

# none of the testcode can work on lunix (no library support)
EXELIST_lunix = \
	notavailable

# omitted: arg-test clock-test clock cpeek-test conio cprintf cursor deb dir-test
# em-test exec-test1 exec-test2 fileio-test ft getopt-test heaptest joy-test
# mouse-test mul-test posixio-test rename-test scanf-test seek ser-test strdup-test
# stroserror-test uname-test
EXELIST_kim1 = \
	minimal \
	div-test \
	moddiv-test \
	strnlen \
	strqtok-test

# Unlisted targets will try to build everything.
# That lets us learn what they cannot build, and what settings
# we need to use for programs that can be built and run.
ifndef EXELIST_$(SYS)
EXELIST_$(SYS) := ${patsubst %.c,%,$(wildcard *.c)}
endif

define SUBDIR_recipe

@+$(MAKE) -C $(dir) --no-print-directory $@

endef # SUBDIR_recipe

# --------------------------------------------------------------------------
# Rules to make the binaries and the disk

testcode: $(EXELIST_$(SYS))
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: generic targettest programs not available for" $(SYS)
endif

disk: $(DISK_$(SYS))

all:

# --------------------------------------------------------------------------
# List of every supported platform
TARGETS :=     \
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
  cbm510       \
  cbm610       \
  creativision \
  cx16         \
  gamate       \
  kim1		   \
  lunix        \
  lynx         \
  nes          \
  osic1p       \
  pce          \
  pet          \
  plus4        \
  sim6502      \
  sim65c02     \
  supervision  \
  sym1         \
  telestrat    \
  vic20

# --------------------------------------------------------------------------
# Rule to make the binaries for every platform

define TARGET_recipe

@echo making targettest for: $(T)
@$(MAKE) -j2 SYS:=$(T)
@$(MAKE) --no-print-directory clean SYS:=$(T)

endef # TARGET_recipe

platforms:
	$(foreach T,$(TARGETS),$(TARGET_recipe))

# --------------------------------------------------------------------------
# some programs link against getsp.o

mouse-test: mouse-test.o getsp.o
	$(LD) $(LDFLAGS) -t $(SYS) -o $@ $^ $(SYS).lib

ifneq ($(SYS),vic20)
ft: ft.o getsp.o
	$(LD) $(LDFLAGS) -t $(SYS) -o $@ $^ $(SYS).lib
endif

# some programs need more memory on the vic20

ifeq ($(SYS),vic20)
ft:  ft.o getsp.o
	$(LD) $(LDFLAGS) -o $@ -C vic20-32k.cfg -m $@.map $^ $(SYS).lib
endif

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all testcode. Needs the c1541 program that comes
# with the VICE emulator.

define D64_WRITE_PRG_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),p >$(NULLDEV)

endef # D64_WRITE_PRG_recipe

define D64_WRITE_SEQ_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),s >$(NULLDEV)

endef # D64_WRITE_SEQ_recipe

testcode.d64: testcode
	@$(C1541) -format testcode,AA d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
#	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_SEQ_recipe))

testcode.d81: testcode
	@$(C1541) -format testcode,AA d81 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_SEQ_recipe))

# --------------------------------------------------------------------------
# Rule to make an Apple II disk with all testcode. Needs the AppleCommander
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

testcode.dsk: testcode
	cp prodos.dsk $@
	$(foreach file,$(EXELIST_$(SYS)),$(DSK_WRITE_BIN_recipe))
#	$(foreach file,$(EMD) $(MOU) $(TGI),$(DSK_WRITE_REL_recipe))

# --------------------------------------------------------------------------
# Rule to make an Atari disk with all testcode. Needs the dir2atr program
# available at http://www.horus.com/~hias/atari/ and the MyDos4534 variant
# of dos.sys and dup.sys.

define ATR_WRITE_recipe

cp "$(subst ?,$(SPACE),$(file))" atr/$(notdir $(file))

endef # ATR_WRITE_recipe

testcode.atr: testcode
	@mkdir atr
	cp "dos.sys" atr/dos.sys
	cp "dup.sys" atr/dup.sys
	@$(foreach file,$(EXELIST_$(SYS)),$(ATR_WRITE_recipe))
#	@$(foreach file,$(EMD) $(MOU) $(TGI),$(ATR_WRITE_recipe))
	$(DIR2ATR) -d -b MyDos4534 3200 $@ atr
	@$(RMDIR) atr

# --------------------------------------------------------------------------
# Clean-up rules

mostlyclean:
	@$(DEL) *.lbl *.map *.o 2>$(NULLDEV)
# we cant use .s since we have asm files in the directory that we want to keep
	@$(DEL) ${patsubst %.c,%.s,$(wildcard *.c)} 2>$(NULLDEV)

clean: mostlyclean
	@$(DEL) $(EXELIST_$(SYS)) $(DISK_$(SYS)) 2>$(NULLDEV)
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))
