
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

all: audiotest.bin lcdtest.bin ctest.bin

audiotest.bin: audiotest.s
	$(CL) -l audiotest.lst -t gamate -o audiotest.bin audiotest.s
lcdtest.bin: lcdtest.s
	$(CL) -l lcdtest.lst -t gamate -o lcdtest.bin lcdtest.s
ctest.bin: ctest.c
	$(CL) -l ctest.lst -t gamate -o ctest.bin ctest.c
nachtm.bin: nachtm.c
	$(CL) -Os -l nachtm.lst -t gamate -o nachtm.bin nachtm.c
	gamate-fixcart nachtm.bin

clean:
	@$(DEL) lcdtest.o audiotest.o ctest.o 2>$(NULLDEV)
	@$(DEL) lcdtest.bin audiotest.bin ctest.bin nachtm.bin 2>$(NULLDEV)
	@$(DEL) audiotest.lst lcdtest.lst ctest.lst 2>$(NULLDEV)
