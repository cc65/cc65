#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= c64

CRT0   	= ../libsrc/$(SYS).o
CLIB	= ../libsrc/$(SYS).lib
CC	= ../src/cc65/cc65
CL	= ../src/cl65/cl65
AS	= ../src/ca65/ca65
LD	= ../src/ld65/ld65
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
# Rules how to make each one of the binaries

EXELIST=ascii fire gunzip65 hello mousedemo nachtm plasma sieve tgidemo

.PHONY:	all
all:   	$(EXELIST)

ascii: 	       	$(CRT0) ascii.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

fire:  	       	$(CRT0) fire.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

gunzip65:      	$(CRT0) gunzip65.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

hello: 	       	$(CRT0) hello.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

mousedemo:     	$(CRT0) mousedemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

nachtm:	       	$(CRT0) nachtm.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

plasma:	       	$(CRT0) plasma.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

sieve: 	       	$(CRT0) sieve.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^

tgidemo:       	$(CRT0) tgidemo.o $(CLIB)
	@$(LD) -t $(SYS) -m $(basename $@).map -o $@ $^


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




