#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= none

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

C_OBJS = dbg.o

S_OBJS = asmtab.o dbgdasm.o dbgdump.o dbgisram.o dbgsupp.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

