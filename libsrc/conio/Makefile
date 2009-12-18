# -*- makefile -*-
#
# makefile for CC65's console library
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
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Rules to help us see what code the compiler and assembler make.

#%.s :	%.c
#	@$(CC) $(CFLAGS) -S $<

%.lst :	%.s
	@$(AS) $(AFLAGS) -l -o /dev/null $<

#--------------------------------------------------------------------------
# Object files

OBJS =	_cursor.o	\
        cprintf.o 	\
	cputhex.o	\
	cputs.o		\
	cscanf.o	\
	cursor.o	\
	scrsize.o       \
        vcprintf.o	\
	vcscanf.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(OBJS)

clean:
	@$(RM) *~ *.lst $(OBJS)

zap:	clean

