#
# Makefile for GEOS lib
# for cc65
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= cchvline.o cgetc.o clrscr.o color.o cputc.o cputs.o cursor.o gotoxy.o kbhit.o\
	revers.o screensize.o where.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
