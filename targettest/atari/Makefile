
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

all: charmapping.xex defdev.xex displaylist.xex mem.xex multi.xex ostype.xex \
	scrcode.com sys.xex

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
