# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= gamate

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

EXELIST_gamate = \
        audiotest.bin lcdtest.bin ctest.bin

ifneq ($(EXELIST_$(SYS)),)
testcode: $(EXELIST_$(SYS))
else
testcode: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: gamate tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'testcode' message
	@echo > $(NULLDEV)
endif

audiotest.bin: audiotest.s
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -l audiotest.lst -t gamate -o audiotest.bin audiotest.s
lcdtest.bin: lcdtest.s
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -l lcdtest.lst -t gamate -o lcdtest.bin lcdtest.s
ctest.bin: ctest.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -l ctest.lst -t gamate -o ctest.bin ctest.c

clean:
	@$(DEL) lcdtest.o audiotest.o ctest.o 2>$(NULLDEV)
	@$(DEL) lcdtest.bin audiotest.bin ctest.bin nachtm.bin 2>$(NULLDEV)
	@$(DEL) audiotest.lst lcdtest.lst ctest.lst 2>$(NULLDEV)
