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

EXELIST_c64 = \
	c64-scpu-test.prg \
	c128-scpu-test.prg \
	c64dtv-test.prg \
	c64-c128-test.prg \
	chameleon-test.prg \
	c65-test.prg \
	turbomaster-test.prg
	
EXELIST_c128 = \
	c128-test.prg
	
ifneq ($(EXELIST_$(SYS)),)
testcode: $(EXELIST_$(SYS))
else
testcode: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: accelerator tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'testcode' message
	@echo > $(NULLDEV)
endif

c64-scpu-test.prg: c64-c128-scpu-test.c
# do not use cl65 to prevent tests from failing randomly because of one process
# deleting the temp files from another
#	$(CL) -t c64 c64-c128-scpu-test.c -o c64-scpu-test.prg
	$(CC) -t c64 c64-c128-scpu-test.c -o c64-scpu-test.s
	$(CL) -t c64 c64-scpu-test.s -o c64-scpu-test.prg

c128-scpu-test.prg: c64-c128-scpu-test.c
# do not use cl65 to prevent tests from failing randomly because of one process
# deleting the temp files from another
#	$(CL) -t c128 c64-c128-scpu-test.c -o c128-scpu-test.prg
	$(CC) -t c128 c64-c128-scpu-test.c -o c128-scpu-test.s
	$(CL) -t c128 c128-scpu-test.s -o c128-scpu-test.prg

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
	@$(DEL) *.s 2>$(NULLDEV)
