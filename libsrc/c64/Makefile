#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS =	_scrsize.o	\
	break.o		\
	crt0.o		\
	cgetc.o		\
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	kbhit.o		\
	mouse.o		\
	readjoy.o      	\
	rs232.o		\
        tgi_mode_table.o\
	write.o

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

