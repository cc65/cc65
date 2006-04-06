#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= c64

# Determine the path to the executables and libraries. If the samples
# directory is part of a complete source tree, use the stuff from this
# source tree, otherwise use the "official" directories.
ifeq "$(wildcard ../src)" ""
# No source tree
CRT0 = $(SYS).o
CLIB = $(SYS).lib
CL   = cl65
CC   = cc65
AS   = as65
LD   = ld65

else
# Samples is part of a complete source tree
CRT0 = ../libsrc/$(SYS).o
CLIB = ../libsrc/$(SYS).lib
CL   = ../src/cl65/cl65
CC   = ../src/cc65/cc65
AS   = ../src/ca65/ca65
LD   = ../src/ld65/ld65
export CC65_INC = ../include
endif

# This one comes with VICE
C1541  	= c1541


# --------------------------------------------------------------------------
# Generic rules

.c.o:
	@echo $<
	@$(CC) -Oirs -T --forget-inc-paths --codesize 500 -g -t $(SYS) -I../include/ $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(basename $<).s


# --------------------------------------------------------------------------
# List of executables. This list could be made target dependent by checking
# $(SYS).

EXELIST	=	ascii 		\
     		diodemo		\
     		fire 		\
     		gunzip65 	\
     		hello 		\
     		mousedemo 	\
     		nachtm 		\
     		plasma 		\
     		sieve 		\
     		tgidemo

# --------------------------------------------------------------------------
# Rules how to make each one of the binaries

.PHONY:	all
all:   	$(EXELIST)

ascii: 	       	$(CRT0) ascii.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

diodemo:        $(CRT0) diodemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

fire:  	       	$(CRT0) fire.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

gunzip65:      	$(CRT0) gunzip65.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

hello: 	       	$(CRT0) hello.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

# The apple machines need the start address adjusted for the mandelbrot demo
ifeq "$(SYS)" "apple2"
mandelbrot:     $(CRT0) mandelbrot.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^
else
ifeq "$(SYS)" "apple2enh"
mandelbrot:     $(CRT0) mandelbrot.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^
else
mandelbrot:     $(CRT0) mandelbrot.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^
endif
endif

# The Apple ][ needs the start address adjusted for the mousedemo
ifeq "$(SYS)" "apple2"
mousedemo:     	$(CRT0) mousedemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^
else
mousedemo:     	$(CRT0) mousedemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^
endif

nachtm:	       	$(CRT0) nachtm.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -Ln $(basename $@).lbl -o $@ $^

plasma:	       	$(CRT0) plasma.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

sieve: 	       	$(CRT0) sieve.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

# The apple machines need the start address adjusted for the tgidemo
ifeq "$(SYS)" "apple2"
tgidemo:       	$(CRT0) tgidemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^
else
ifeq "$(SYS)" "apple2enh"
tgidemo:       	$(CRT0) tgidemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map --start-addr 0x4000 -o $@ $^
else
tgidemo:       	$(CRT0) tgidemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^
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
	    $(C1541) -attach $@ -write $$exe > /dev/null;\
	done;\
	for tgi in ../libsrc/$(SYS)*.tgi; do\
	    $(C1541) -attach $@ -write $$tgi > /dev/null;\
	done;

# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST) samples.d64




