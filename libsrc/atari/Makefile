#
# makefile for CC65 Atari runtime library
#
# NUMDRVS - number of supported drives (max. 16)

ATARIDEFS = -DNUMDRVS=4 -DUCASE_FILENAME=1

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $(ATARIDEFS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $(ATARIDEFS) $<

OBJS =	_scrsize.o   \
	break.o      \
	cclear.o     \
	cgetc.o      \
	chline.o     \
	clock.o      \
	close.o      \
	clrscr.o     \
	color.o      \
	cputc.o      \
	crt0.o 	     \
	ctype.o      \
	cvline.o     \
	dio_cts.o    \
	dio_stc.o    \
	diopncls.o   \
	dioread.o    \
	diowrite.o   \
	diowritev.o  \
	fdtable.o    \
	getargs.o    \
	getfd.o      \
	gotox.o      \
	gotoy.o      \
	gotoxy.o     \
	graphics.o   \
	graphuse.o   \
	kbhit.o      \
	lseek.o      \
	mouse.o      \
	mul40.o      \
	open.o 	     \
	oserror.o    \
	ostype.o     \
	read.o 	     \
	readjoy.o    \
	remove.o     \
	revers.o     \
	rs232.o	     \
	rwcommon.o   \
	savevec.o    \
	scroll.o     \
	setcolor.o   \
	siocall.o    \
	tvtype.o     \
	ucase_fn.o   \
	where.o      \
	write.o


all:   	$(OBJS)

clean:
	@rm -f $(OBJS)

