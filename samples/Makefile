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
	@$(CC) -g -Oirs -t $(SYS) -I../include/ $<
	@$(AS) $(basename $<).s

.s.o:
	@echo $<
	@$(AS) $(basename $<).s


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




