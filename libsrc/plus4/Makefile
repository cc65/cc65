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
       	cgetc.o	       	\
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	crt0.o		\
        kacptr.o        \
        kbasin.o        \
	kbhit.o		\
        kbsout.o        \
        kchkin.o        \
        kciout.o        \
        kckout.o        \
        kclall.o        \
        kclose.o        \
        kclrch.o        \
        kiobase.o       \
        klisten.o       \
        kload.o         \
        kopen.o         \
        krdtim.o        \
        kreadst.o       \
        ksave.o         \
        ksetlfs.o       \
        ksetnam.o       \
        ksettim.s       \
        ktalk.o         \
        kunlsn.o        \
        kuntlk.o        \
        randomize.o     \
	readjoy.o	\
	tgi_mode_table.o

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

