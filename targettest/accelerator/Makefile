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

all:	c64-scpu-test.prg c128-scpu-test.prg c64dtv-test.prg \
	c64-c128-test.prg c128-test.prg chameleon-test.prg \
	c65-test.prg turbomaster-test.prg

c64-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c64 c64-c128-scpu-test.c -o c64-scpu-test.prg

c128-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c128 c64-c128-scpu-test.c -o c128-scpu-test.prg

c64dtv-test.prg: c64dtv-test.c
	$(CL) -t c64 c64dtv-test.c -o c64dtv-test.prg

c64-c128-test.prg: c64-c128-test.c
	$(CL) -t c64 c64-c128-test.c -o c64-c128-test.prg

c128-test.prg: c64-c128-test.c
	$(CL) -t c128 c64-c128-test.c -o c128-test.prg

chameleon-test.prg: chameleon-test.c
	$(CL) -t c64 chameleon-test.c -o chameleon-test.prg

c65-test.prg: c65-test.c
	$(CL) -t c64 c65-test.c -o c65-test.prg

turbomaster-test.prg: turbomaster-test.c
	$(CL) -t c64 turbomaster-test.c -o turbomaster-test.prg

clean:
	@$(DEL) *.prg 2>$(NULLDEV)
