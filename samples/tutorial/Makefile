
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
else
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
endif 

EXELIST_atari2600 = \
	notavailable

EXELIST_bbc = \
	notavailable

EXELIST_creativision = \
	notavailable

EXELIST_gamate = \
	notavailable

EXELIST_geos-cbm = \
	notavailable

EXELIST_geos-apple = \
	notavailable

EXELIST_lunix = \
	notavailable

EXELIST_lynx = \
	notavailable

EXELIST_nes = \
	notavailable

EXELIST_osic1p = \
	notavailable

EXELIST_pce = \
	notavailable

EXELIST_supervision = \
	notavailable

# Unlisted targets will try to build everything.
# That lets us learn what they cannot build, and what settings
# we need to use for programs that can be built and run.
ifndef EXELIST_$(SYS)
EXELIST_$(SYS) := ${patsubst %.c,%,$(wildcard *.c)}
endif

samples: $(EXELIST_$(SYS))

hello: hello.c text.s
	$(CL) -t $(SYS) -o hello hello.c text.s

# empty target used to skip systems that will not work with any program in this dir
notavailable:
	@echo "warning: tutorial sample not available for" $(SYS)
clean:
	@$(DEL) hello 2>$(NULLDEV)
