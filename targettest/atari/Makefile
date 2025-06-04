# ---- Display info during parsing phase ----
$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~)
$(info ~~~ Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(MAKECMDGOALS),)
  $(info ~~~ Invoked target: (default))
else
  $(info ~~~ Invoked target: $(MAKECMDGOALS))
endif
$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~)


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= atari

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

EXELIST_atari = \
        charmapping.xex \
        defdev.xex \
        displaylist.xex \
        mem.xex \
        multi.xex \
        ostype.xex \
        scrcode.com \
        sound.xex \
        sys.xex

ifneq ($(EXELIST_$(SYS)),)
testcode: $(EXELIST_$(SYS))
else
testcode: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: atari tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'testcode' message
	@echo > $(NULLDEV)
endif

charmapping.xex: charmapping.c
	$(CL) -t atari -o charmapping.xex charmapping.c
defdev.xex: defdev.c
	$(CL) -t atari -o defdev.xex defdev.c
displaylist.xex: displaylist.c
	$(CL) -t atari -o displaylist.xex displaylist.c
mem.xex: mem.c ../getsp.s
	$(CL) -t atari -o mem.xex mem.c ../getsp.s
multi.xex: multi-xex.s multi-xex.cfg
	$(CL) -t atari -C multi-xex.cfg multi-xex.s -o multi.xex
ostype.xex: ostype.c
	$(CL) -t atari -o ostype.xex ostype.c
scrcode.com: scrcode.s
#	ca65 -t atari -o scrcode.o scrcode.s
#	ld65 -C atari-asm.cfg -o scrcode.com scrcode.o
	$(CL) -t atari -C atari-asm.cfg -o scrcode.com scrcode.s
sys.xex: sys.c
	$(CL) -t atari -o sys.xex sys.c
sound.xex: sound.c
	$(CL) -t atari -o sound.xex sound.c
clean:
	@$(DEL) charmapping.xex 2>$(NULLDEV)
	@$(DEL) defdev.xex 2>$(NULLDEV)
	@$(DEL) displaylist.xex 2>$(NULLDEV)
	@$(DEL) mem.xex 2>$(NULLDEV)
	@$(DEL) multi.xex 2>$(NULLDEV)
	@$(DEL) ostype.xex 2>$(NULLDEV)
	@$(DEL) scrcode.o 2>$(NULLDEV)
	@$(DEL) scrcode.com 2>$(NULLDEV)
	@$(DEL) sys.xex 2>$(NULLDEV)
	@$(DEL) sound.xex 2>$(NULLDEV)
