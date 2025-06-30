# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= lynx

# Just the usual way to find out if we're
# using cmd.exe to execute make rules.
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

EXELIST_lynx = \
	hello.lnx \
	mandelbrot.lnx \
	tgidemo.lnx

.PHONY: samples clean

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
else
samples:
# recipe used to skip systems that will not work with any program in this dir
ifeq ($(MAKELEVEL),0)
	@echo "info: Lynx tests not available for" $(SYS)
else
# Suppress the "nothing to be done for 'samples' message.
	@echo "" > $(NULLDEV)
endif
endif

.SUFFIXES:
.SUFFIXES: .c .lnx

%.lnx : %.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -Oris -m $*.map -o $@ $<

clean:
	@$(DEL) *.o *.map *.lnx 2>$(NULLDEV)
