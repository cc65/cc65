#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

OBJS =	_cursor.o	\
        cprintf.o 	\
	cputhex.o	\
	cputs.o		\
	cursor.o	\
	scrsize.o       \
        vcprintf.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(OBJS)

clean:
	@rm -f *~ $(OBJS)

zap:	clean

