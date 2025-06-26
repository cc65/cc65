# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# Sample makefile using a preprocessor against info files
# and the --sync-lines option

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
  DA = $(CC65_HOME)/bin/da65
else
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
  DA := $(if $(wildcard ../../bin/da65*),../../bin/da65,da65)
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
endif

CPP = cpp
#CPPFLAGS = -DTEST_ERROR

ASMS = fixed.s bank0.s bank1.s
DAIS = fixed.dai bank0.dai bank1.dai

.SUFFIXES: .da .dai .s

samples: image.bin $(ASMS)

$(DAIS): fixed.da

.da.dai:
	$(CPP) -o $@ $(CPPFLAGS) $<

.dai.s:
	$(DA) --sync-lines -o $@ -i $< image.bin

image.bin: image.s image.cfg
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t none -C image.cfg -o image.bin image.s

clean:
	@$(DEL) $(ASMS) 2>$(NULLDEV)
	@$(DEL) $(DAIS) 2>$(NULLDEV)
	@$(DEL) image.bin 2>$(NULLDEV)

