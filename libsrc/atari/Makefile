#
# makefile for CC65 Atari runtime library
#

ATARIDEFS = -DDIRECT_SCREEN

.SUFFIXES: .o .s .c

%.o:   	%.c
	@echo $<
	@$(CC) $(CFLAGS) $(ATARIDEFS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@echo $<
	@$(AS) -g -o $@ $(AFLAGS) $(ATARIDEFS) $<

C_OBJS =

S_OBJS = crt0.o kbhit.o conio.o clrscr.o cputc.o ctype.o chline.o cvline.o \
	 color.o gotoxy.o cclear.o revers.o readjoy.o break.o where.o write.o \
	 gotox.o gotoy.o savevec.o rwcommon.o cgetc.o read.o getargs.o close.o \
	 open.o oserror.o fdtable.o setcolor.o scroll.o mul40.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)
	@rm -f crt0.o

