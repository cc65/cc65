# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= c64

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
  SP = $(CC65_HOME)/bin/sp65
else
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
  SP := $(if $(wildcard ../../bin/sp65*),../../bin/sp65,sp65)
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
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
endif

DISK_$(SYS) = samples.d64

EXELIST_c64 = \
        fire \
        plasma \
        nachtm \
        hello

EXELIST_c128 = \
        fire \
        plasma \
        nachtm \
        hello

EXELIST_cbm510 = \
        fire \
        plasma \
        nachtm

EXELIST_cbm610 = \
        nachtm

EXELIST_plus4 = \
        plasma \
        hello

EXELIST_c16 = \
        hello

EXELIST_pet = \
        notavailable

EXELIST_vic20 = \
        hello

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
else
samples: notavailable
endif

disk: $(DISK_$(SYS))

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: cbm samples not available for" $(SYS)
else
# suppress the "nothing to be done for 'samples' message
	@echo > $(NULLDEV)
endif

fire: fire.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o fire -m fire.map fire.c
plasma: plasma.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o plasma -m plasma.map plasma.c
nachtm: nachtm.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o nachtm -m nachtm.map nachtm.c
hello: hello-asm.s
	$(if $(QUIET),echo $(SYS):$@)
        # Use separate assembler ...
	$(AS) -t $(SYS) hello-asm.s
        # ... and linker commands ...
	$(LD) -C $(SYS)-asm.cfg -o hello -m hello-asm.map -u __EXEHDR__ hello-asm.o $(SYS).lib
	@$(DEL) hello-asm.o 2>$(NULLDEV)
        # ... or compile & link utility
#	$(CL) -C $(SYS)-asm.cfg -o hello -m hello-asm.map -u __EXEHDR__ hello-asm.s


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
	$(if $(QUIET),echo $(SYS):$@)
	@$(C1541) -format "samples,00" d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
#	$(foreach file,$(OVERLAYLIST),$(D64_WRITE_PRG_recipe))
#	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_SEQ_recipe))

clean:
	@$(DEL) $(EXELIST_$(SYS)) 2>$(NULLDEV)
	@$(DEL) *.map 2>$(NULLDEV)
	@$(DEL) $(DISK_$(SYS)) 2>$(NULLDEV)
