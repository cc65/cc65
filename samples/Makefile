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
	@$(CC) -Oirs -T --codesize 500 -g -t $(SYS) -I../include/ $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(basename $<).s


# --------------------------------------------------------------------------
# Rules how to make each one of the binaries

EXELIST=ascii fire hello mousedemo nachtm plasma sieve tgidemo

.PHONY:	all
all:   	$(EXELIST)

ascii: 		$(CRT0) ascii.o $(CLIB)
	@$(LD) -t $(SYS) -m ascii.map -Ln ascii.lbl -o $@ $^

fire: 		$(CRT0) fire.o $(CLIB)
	@$(LD) -t $(SYS) -m fire.map -Ln fire.lbl -o $@ $^

hello: 		$(CRT0) hello.o $(CLIB)
	@$(LD) -t $(SYS) -m hello.map -Ln hello.lbl -o $@ $^

mousedemo:     	$(CRT0) mousedemo.o $(CLIB)
	@$(LD) -t $(SYS) -m mousedemo.map -Ln mousedemo.lbl -o $@ $^

nachtm:		$(CRT0) nachtm.o $(CLIB)
	@$(LD) -t $(SYS) -vm -m nachtm.map -Ln nachtm.lbl -o $@ $^

plasma:		$(CRT0) plasma.o $(CLIB)
	@$(LD) -t $(SYS) -m plasma.map -Ln nachtm.lbl -o $@ $^

sieve: 		$(CRT0) sieve.o $(CLIB)
	@$(LD) -t $(SYS) -m sieve.map -Ln sieve.lbl -o $@ $^

tgidemo:	$(CRT0) tgidemo.o $(CLIB)
	@$(LD) -t $(SYS) -m tgidemo.map -Ln tgidemo.lbl -o $@ $^


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
	rm -f *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	rm -f $(EXELIST) samples.d64




