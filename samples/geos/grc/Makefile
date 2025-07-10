# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= geos-cbm

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
  GRC = $(CC65_HOME)/bin/grc65
else
  AS := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../../bin/cc65*),../../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../../bin/cl65*),../../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)
  GRC := $(if $(wildcard ../../../bin/grc65*),../../../bin/grc65,grc65)
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
endif

EXELIST_geos-cbm = \
	test.s \
	vlir.cvt

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
else
samples:
  ifeq ($(MAKELEVEL),0)
	@echo "info: grc sample not available for" $(SYS)
  else
# suppress the "nothing to be done for 'samples' message
	@echo > $(NULLDEV)
  endif
endif

test.s: test.grc
	$(if $(QUIET),echo $(SYS):$@)
	$(GRC) -s test.s test.grc

vlir.cvt: vlir.grc vlir0.s vlir1.s vlir2.s
	$(if $(QUIET),echo $(SYS):$@)
# using separate calls here for demonstration purposes:
	$(GRC) -t $(SYS) -s vlir.s vlir.grc
	$(AS) -t $(SYS) vlir.s
	$(AS) -t $(SYS) vlir0.s
	$(AS) -t $(SYS) vlir1.s
	$(AS) -t $(SYS) vlir2.s
	$(LD) -t $(SYS) -o vlir.cvt vlir.o vlir0.o vlir1.o vlir2.o $(SYS).lib

# you can also do the above in one command:
#	$(CL) -t $(SYS) -o vlir.cvt vlir.grc vlir0.s vlir1.s vlir2.s

clean:
	@$(DEL) test.s test.h 2>$(NULLDEV)
	@$(DEL) vlir.s vlir.cvt vlir.c vlir.h 2>$(NULLDEV)
	@$(DEL) *.o 2>$(NULLDEV)
