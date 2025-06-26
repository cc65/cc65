# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= pce

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

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
endif

.PHONY: all clean test

# Size of cartridge to generate.
# Possible values:
#  8K = 0x2000
# 16K = 0x4000
# 32K = 0x8000
CARTSIZE := 0x2000

ifeq (${CARTSIZE},0x8000)
COUNT := 3
else
COUNT := 1
endif

EXELIST_pce = \
        conio.bin

ifneq ($(EXELIST_$(SYS)),)
testcode: $(EXELIST_$(SYS))
else
testcode: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: pce tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'testcode' message
	@echo > $(NULLDEV)
endif

%.bin: %.c
	$(if $(QUIET),echo $@)
	$(CL) -t pce $< -Wl -D__CARTSIZE__=${CARTSIZE} -m $*.map -o $@
ifndef QUIET
	@echo "use 'make conio.pce' to produce a .pce file using dd"
endif

%.pce: %.bin
	dd if=$< bs=8K skip=${COUNT} > $@
	dd if=$< bs=8K count=${COUNT} >> $@

clean:
	@$(DEL) conio.o 2>$(NULLDEV)
	@$(DEL) conio.pce 2>$(NULLDEV)
	@$(DEL) conio.bin 2>$(NULLDEV)
	@$(DEL) conio.map 2>$(NULLDEV)
