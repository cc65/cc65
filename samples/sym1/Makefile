# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= sym1

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

EXELIST_sym1 = \
        symHello.bin symTiny.bin symDisplay.bin symIO.bin symNotepad.bin symExtendedMemory.bin

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
else
samples: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: sym1 tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'samples' message
	@echo > $(NULLDEV)
endif

symHello.bin: symHello.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -O -o symHello.bin symHello.c

symTiny.bin: symTiny.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -O -o symTiny.bin symTiny.c

symDisplay.bin: symDisplay.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -O -o symDisplay.bin symDisplay.c

symIO.bin: symIO.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -C sym1-32k.cfg -O -o symIO.bin symIO.c

symNotepad.bin: symNotepad.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -C sym1-32k.cfg -O -o symNotepad.bin symNotepad.c

symExtendedMemory.bin: symExtendedMemory.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t sym1 -C sym1-32k.cfg -O -o symExtendedMemory.bin symExtendedMemory.c


clean:
	@$(DEL) symHello.bin 2>$(NULLDEV)
	@$(DEL) symTiny.bin 2>$(NULLDEV)
	@$(DEL) symDisplay.bin 2>$(NULLDEV)
	@$(DEL) symIO.bin 2>$(NULLDEV)
	@$(DEL) symNotepad.bin 2>$(NULLDEV)
	@$(DEL) symExtendedMemory.bin 2>$(NULLDEV)
