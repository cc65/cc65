#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= c64

# Determine the path to the executables and libraries. If the samples
# directory is part of a complete source tree, use the stuff from that
# source tree; otherwise, use the "install" directories.
ifeq "$(wildcard ../src)" ""
# No source tree
MOUS = /usr/lib/cc65/mou/$(SYS)*.mou
TGI  = /usr/lib/cc65/tgi/$(SYS)*.tgi
ifneq "$(wildcard /usr/local/lib/cc65)" ""
MOUS = /usr/local/lib/cc65/mou/$(SYS)*.mou
TGI  = /usr/local/lib/cc65/tgi/$(SYS)*.tgi
endif
ifdef CC65_HOME
MOUS = $(CC65_HOME)/mou/$(SYS)*.mou
TGI  = $(CC65_HOME)/tgi/$(SYS)*.tgi
endif
CLIB = --lib $(SYS).lib
CL   = cl65
CC   = cc65
AS   = ca65
LD   = ld65

else
# "samples/" is a part of a complete source tree.
CA65_INC = ../asminc
CC65_INC = ../include
LD65_CFG = ../src/ld65/cfg
LD65_LIB = ../libsrc
LD65_OBJ = ../libsrc
MOUS = ../libsrc/$(SYS)*.mou
TGI  = ../libsrc/$(SYS)*.tgi
CLIB = ../libsrc/$(SYS).lib
CL   = ../src/cl65/cl65
CC   = ../src/cc65/cc65
AS   = ../src/ca65/ca65
LD   = ../src/ld65/ld65

MY_INC = --forget-inc-paths -I . -I $(CC65_INC)
MY_ASM = --forget-inc-paths -I . -I $(CA65_INC)
endif

# This one comes with VICE
C1541  	= c1541


# --------------------------------------------------------------------------
# Generic rules

.c.o:
	@echo $<
	@$(CC) $(MY_INC) -Oirs --codesize 500 -T -g -t $(SYS) $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(MY_ASM) -t $(SYS) $<

.o:
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^ $(CLIB)


# --------------------------------------------------------------------------
# List of executables. This list could be made target dependent by checking
# $(SYS).

EXELIST	=	ascii 		\
     		diodemo		\
     		fire 		\
     		gunzip65 	\
     		hello 		\
		mandelbrot	\
     		mousedemo 	\
     		nachtm 		\
     		plasma 		\
     		sieve 		\
     		tgidemo

# --------------------------------------------------------------------------
# Rules how to make each one of the binaries

.PHONY:	all
all:   	$(EXELIST)

ascii:		ascii.o

diodemo:	diodemo.o

fire:		fire.o

gunzip65:	gunzip65.o

hello:		hello.o

# The Apple machines need the start address adjusted for the mandelbrot demo
ifeq "$(SYS)" "apple2"
mandelbrot:	mandelbrot.o
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^ $(CLIB)
else
ifeq "$(SYS)" "apple2enh"
mandelbrot:	mandelbrot.o
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^ $(CLIB)
else
mandelbrot:	mandelbrot.o
endif
endif

# The Apple ][ needs the start address adjusted for the mousedemo
ifeq "$(SYS)" "apple2"
mousedemo:	mousedemo.o
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^ $(CLIB)
else
mousedemo:	mousedemo.o
endif

nachtm:		nachtm.o

plasma:		plasma.o

sieve:		sieve.o

# The Apple machines need the start address adjusted for the tgidemo
ifeq "$(SYS)" "apple2"
tgidemo:	tgidemo.o
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^ $(CLIB)
else
ifeq "$(SYS)" "apple2enh"
tgidemo:	tgidemo.o
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^ $(CLIB)
else
tgidemo:	tgidemo.o
endif
endif

# --------------------------------------------------------------------------
# Rule to make a disk with all samples. Needs the c1541 program that comes
# with the VICE emulator.

.PHONY:	disk
disk:  	samples.d64

samples.d64:	all
	@$(C1541) -format samples,AA  d64 $@ > /dev/null
	@for exe in $(EXELIST); do\
	    $(C1541) -attach $@ -write $$exe > /dev/null || exit $$?;\
	done
	@for mod in $(TGI) $(MOUS); do\
	    $(C1541) -attach $@ -write $$mod > /dev/null || exit $$?;\
	done

# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST) samples.d64


