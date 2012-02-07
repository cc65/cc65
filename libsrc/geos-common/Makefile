#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= geos-cbm

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../$(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) -o $(notdir $(*).s) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Directories

DIRS =	common		\
	conio		\
	dlgbox		\
	disk		\
	file		\
	graph		\
	memory		\
	menuicon	\
	mousesprite	\
	process		\
	runtime		\
	system

#--------------------------------------------------------------------------
# Directives

include $(addsuffix /Makefile, $(DIRS))
vpath %.c $(DIRS)
vpath %.s $(DIRS)

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

zap:	clean
