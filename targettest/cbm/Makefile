# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= c64

# This one comes with the VICE emulator.
# See http://vice-emu.sourceforge.net/
C1541 ?= c1541

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
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
endif

EXELIST_c64 = \
        petscii.prg \
        cbmdir-test.prg \
        cbmread.prg

EXTRALIST_c64 = \
	read-0 \
	read-1 \
	read-8

EXELIST_vic20 = \
        cbmdir-test.prg

DISK_c64       = testcode.d64
DISK_vic20     = testcode.d64

ifneq ($(EXELIST_$(SYS)),)
testcode: $(EXELIST_$(SYS))
else
testcode: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: cbm tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'testcode' message
	@echo > $(NULLDEV)
endif

ifeq ($(SYS),c64)
petscii.prg: petscii.c
	$(CL) -t $(SYS) -O -o petscii.prg petscii.c
else
petscii.prg:
endif

cbmdir-test.prg: cbmdir-test.c
ifeq ($(SYS),vic20)
	$(CL) -t $(SYS) -C vic20-32k.cfg -Oris -o $@ $<
else
	$(CL) -t $(SYS) -Oris -o $@ $<
endif

cbmread.prg: cbmread.c
ifeq ($(SYS),vic20)
	$(CL) -t $(SYS) -C vic20-32k.cfg -Oris -o $@ $<
else
	$(CL) -t $(SYS) -Oris -o $@ $<
endif

# --------------------------------------------------------------------------
# Rule to make a CBM disk with all testcode. Needs the c1541 program that comes
# with the VICE emulator.

define D64_WRITE_PRG_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" "$(subst ?,$(SPACE),$(subst .prg,,$(notdir $(file))))",p >$(NULLDEV)

endef # D64_WRITE_PRG_recipe

define D64_WRITE_SEQ_recipe

$(C1541) -attach $@ -write "$(subst ?,$(SPACE),$(file))" $(notdir $(file)),s >$(NULLDEV)

endef # D64_WRITE_SEQ_recipe

disk: $(DISK_$(SYS))

testcode.d64: testcode
	@$(C1541) -format "testcode,00" d64 $@ >$(NULLDEV)
	$(foreach file,$(EXELIST_$(SYS)),$(D64_WRITE_PRG_recipe))
	$(foreach file,$(EXTRALIST_$(SYS)),$(D64_WRITE_SEQ_recipe))
#	$(foreach file,$(EMD) $(MOU) $(TGI),$(D64_WRITE_SEQ_recipe))

clean:
	@$(DEL) *.lbl petscii.prg cbmdir-test.prg 2>$(NULLDEV)
	@$(DEL) $(DISK_c64)
	@$(DEL) $(DISK_vic20)
