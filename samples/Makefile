#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= c64

CRT0	= ../lib/$(SYS).o
CLIB	= ../lib/$(SYS).lib
CC	= ../cc65/cc65
CL	= ../cl65/cl65
AS	= ../binutils/ca65/ca65
LD	= ../binutils/ld65/ld65
C1541  	= c1541


.c.o:
	@echo $<
	@$(CL) -c -Oirs -t $(SYS) -I../include/ $<

.s.o:
	@echo $<
	@$(CL) -c $(basename $<).s


.PHONY:	all
all:   	nachtm hello sieve

nachtm:	$(CRT0) nachtm.o $(CLIB)
	@$(LD) -t $(SYS) -m nachtm.map -Ln nachtm.lbl -o $@ $^

hello: 	$(CRT0) hello.o $(CLIB)
	@$(LD) -t $(SYS) -m hello.map -Ln hello.lbl -o $@ $^

sieve: 	$(CRT0) sieve.o $(CLIB)
	@$(LD) -t $(SYS) -m sieve.map -Ln sieve.lbl -o $@ $^

.PHONY:	disk
disk:	c64.d64

c64.d64:    	all
	$(C1541) < c1541.rsp

.PHONY:	clean
clean:
	rm -f *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	rm -f nachtm hello




