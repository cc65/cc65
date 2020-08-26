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
  AS := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../../bin/cc65*),../../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../../bin/cl65*),../../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)
  DA := $(if $(wildcard ../../../bin/da65*),../../../bin/da65,da65)
endif

CPP = env LANG=C cpp
CPPFLAGS = # -DTEST_ERROR

ASMS = fixed.s bank0.s bank1.s
DAIS = fixed.dai bank0.dai bank1.dai

.SUFFIXES: .da .dai .s
.PHONY: all clean maintainer-clean
.SECONDARY: $(DAIS)

.da.dai:
	$(CPP) -o $@ $(CPPFLAGS) $<

.dai.s:
	$(DA) --sync-lines -o $@ -i $< image.bin

all: $(ASMS)

clean:
	$(RM) $(ASMS)

maintainer-clean: clean
	$(RM) $(DAIS)

$(DAIS): fixed.da
