#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS =	_cursor.o	\
        cprintf.o 	\
	cputhex.o	\
	cputs.o		\
	cursor.o	\
	scrsize.o       \
        vcprintf.o

all:  	$(OBJS)

clean:
	@rm -f *~ $(OBJS)
