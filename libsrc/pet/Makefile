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
	cgetc.o		\
        checkst.o       \
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	crt0.o		\
	kbhit.o         \
        kbsout.o        \
        kchkin.o        \
        kckout.o        \
        kclose.o        \
        kernal.o        \
        kopen.o         \
        krdtim.o        \
        kreadst.o       \
        ksetlfs.o       \
        ksetnam.o       \
        randomize.o

all:	$(OBJS)

clean:
	@rm -f $(OBJS)
