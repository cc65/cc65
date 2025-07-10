# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= kim1

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

EXELIST_kim1 = 	\
		kimHello.bin \
		kimSieve.bin \
		kimLife.bin \
		kimTest.bin \
		kimGFX.bin

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
else
samples: notavailable
endif

# empty target used to skip systems that will not work with any program in this dir
notavailable:
ifeq ($(MAKELEVEL),0)
	@echo "info: kim1 tests not available for" $(SYS)
else
# suppress the "nothing to be done for 'samples' message
	@echo > $(NULLDEV)
endif

subs.o: subs.asm
	$(AS) subs.asm -o subs.o

ramfont.o: ramfont.asm
	$(AS) ramfont.asm -o ramfont.o

kimLife.bin: kimLife.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t kim1 -C kim1-60k.cfg -Oi -o kimLife.bin kimLife.c

kimTest.bin: kimTest.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t kim1 -C kim1-60k.cfg -Oi -o kimTest.bin kimTest.c

kimGFX.bin: kimGFX.c subs.o ramfont.o
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t kim1 --listing kimGFX.lst -C kim1-mtuE000.cfg -o kimGFX.bin kimGFX.c subs.o ramfont.o -Ln kimgfx.lbl

kimSieve.bin: kimSieve.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t kim1 -C kim1-60k.cfg -O -o kimSieve.bin kimSieve.c

kimHello.bin: kimHello.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t kim1 -O -o kimHello.bin kimHello.c

# To build an intel-format file for the CORSHAM SD card reader

kimLife.hex: kimLife.bin
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimLife.bin  -binary -offset 0x2000 -o kimLife.hex  -Intel -address-length=2

kimTest.hex: kimTest.bin
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimTest.bin  -binary -offset 0x2000 -o kimTest.hex  -Intel -address-length=2

kimGFX.hex: kimGFX.bin ramfont.o
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimGFX.bin  -binary -offset 0x2000 -o kimGFX.hex  -Intel -address-length=2

# To build a paper tape file for uploading to the KIM-1 via terminal

kimLife.ptp: kimLife.bin
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimLife.bin -binary -offset 0x2000 -o kimLife.ptp -MOS_Technologies

kimGFX.ptp: kimGFX.bin
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimGFX.bin -binary -offset 0x2000 -o kimGFX.ptp -MOS_Technologies

kimTest.ptp: kimTest.bin
	$(if $(QUIET),echo $(SYS):$@)
	srec_cat kimTest.bin -binary -offset 0x2000 -o kimTest.ptp -MOS_Technologies

clean:
	@$(DEL) kimSieve.bin 2>$(NULLDEV)
	@$(DEL) kimHello.bin 2>$(NULLDEV)
	@$(DEL) kimLife.bin  2>$(NULLDEV)
	@$(DEL) kimLife.ptp  2>$(NULLDEV)
	@$(DEL) kimLife.hex  2>$(NULLDEV)
	@$(DEL) kimTest.bin  2>$(NULLDEV)
	@$(DEL) kimTest.ptp  2>$(NULLDEV)
	@$(DEL) kimTest.hex  2>$(NULLDEV)
	@$(DEL) kimGFX.bin   2>$(NULLDEV)
	@$(DEL) kimGFX.ptp   2>$(NULLDEV)
	@$(DEL) kimGFX.hex   2>$(NULLDEV)
	@$(DEL) kimgfx.lbl   2>$(NULLDEV)
	@$(DEL) kimGFX.lst   2>$(NULLDEV)
	@$(DEL) subs.o       2>$(NULLDEV)
	@$(DEL) ramfont.o    2>$(NULLDEV)



