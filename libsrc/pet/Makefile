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
        kplot.o         \
        krdtim.o        \
        kreadst.o       \
        ksetlfs.o       \
        ksetnam.o       \
        randomize.o	\
	revers.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

zap:	clean

