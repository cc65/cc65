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


.c.o:
	@echo $<
	@$(CC) -Oirs -T --codesize 500 -g -t $(SYS) -I../include/ $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(basename $<).s


.PHONY:	all
all:   	nachtm hello sieve mousedemo

nachtm:	$(CRT0) nachtm.o $(CLIB)
	@$(LD) -t $(SYS) -m nachtm.map -Ln nachtm.lbl -o $@ $^

hello: 	$(CRT0) hello.o $(CLIB)
	@$(LD) -t $(SYS) -m hello.map -Ln hello.lbl -o $@ $^

sieve: 	$(CRT0) sieve.o $(CLIB)
	@$(LD) -t $(SYS) -m sieve.map -Ln sieve.lbl -o $@ $^

mousedemo:     	$(CRT0) mousedemo.o $(CLIB)
	@$(LD) -t $(SYS) -m mousedemo.map -Ln mousedemo.lbl -o $@ $^

.PHONY:	disk
disk:  	samples.d64

samples.d64:	all
	$(C1541) -format samples,AA  d64 $@
	$(C1541) -attach $@ -write nachtm -write hello -write sieve -write mousedemo

.PHONY:	clean
clean:
	rm -f *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	rm -f nachtm hello sieve mousedemo samples.d64




