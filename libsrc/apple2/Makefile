#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =

S_OBJS=	_scrsize.o	\
	break.o		\
	cclear.o	\
	cgetc.o		\
	chline.o	\
	clrscr.o	\
	color.o		\
	cputc.o		\
	crt0.o		\
	ctype.o		\
	cvline.o	\
	kbhit.o		\
        randomize.o     \
	read.o		\
	revers.o	\
        systime.o       \
	where.o		\
	write.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS)


