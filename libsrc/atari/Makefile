#
# makefile for CC65 Atari runtime library
#
# NUMDRVS - number of supported drives (max. 16)

ATARIDEFS = -DNUMDRVS=4

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $(ATARIDEFS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $(ATARIDEFS) $<

C_OBJS =

S_OBJS = crt0.o _scrsize.o kbhit.o clrscr.o cputc.o ctype.o chline.o cvline.o \
	 color.o gotoxy.o cclear.o revers.o readjoy.o break.o where.o write.o \
	 gotox.o gotoy.o savevec.o rwcommon.o cgetc.o read.o getargs.o close.o \
	 open.o oserror.o fdtable.o setcolor.o scroll.o mul40.o graphuse.o \
	 ostype.o clock.o lseek.o dioread.o diowrite.o siocall.o diowritev.o \
	 dio_cts.o dio_stc.o diopncls.o getfd.o mouse.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS) crt0.o

