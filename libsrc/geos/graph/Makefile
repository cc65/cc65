#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= drawline.o drawpoint.o framerectangle.o hlineregs.o horizontalline.o\
	  imprintrectangle.o invertline.o invertrectangle.o pointregs.o recoverline.o\
	  recoverrectangle.o rectangle.o initdrawwindow.o setpattern.o testpoint.o verticalline.o\
	  putchar.o putdecimal.o putstring.o usesystemfont.o\
	  getcharwidth.o loadcharset.o bitmapup.o bitmapregs.o bitmapclip.o bitotherclip.o\
	  graphicsstring.o getintcharint.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
