#
# makefile for CC65 runtime library (C16)
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS =	_scrsize.o	\
	break.o		\
       	cgetc.o	       	\
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	crt0.o		\
        get_tv.o        \
	kbhit.o		\
        kernal.o        \
        randomize.o     \
	readjoy.o       \
        revers.o        

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

