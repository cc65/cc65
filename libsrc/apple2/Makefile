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

OBJS=	_scrsize.o	\
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
        mainargs.o      \
        randomize.o     \
	read.o		\
	revers.o	\
        systime.o       \
	where.o		\
	write.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

zap:	clean

