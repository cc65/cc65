#
# Makefile for GEOS lib
# for cc65
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= cclear.o chline.o cvline.o cgetc.o clrscr.o color.o\
	cputc.o cpputs.o cursor.o gotoxy.o kbhit.o revers.o\
	where.o _scrsize.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
