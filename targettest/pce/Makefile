
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
  AS := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../../bin/cc65*),../../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../../bin/cl65*),../../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)
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

all: conio.pce

%.pce: %.bin
	dd if=$< bs=8K skip=${COUNT} > $@
	dd if=$< bs=8K count=${COUNT} >> $@

%.bin: %.c
	$(CL) -t pce $< -Wl -D__CARTSIZE__=${CARTSIZE} -m $*.map -o $@

clean:
	@$(DEL) conio.o conio.??? 2>$(NULLDEV)
