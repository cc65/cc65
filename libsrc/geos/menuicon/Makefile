#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= domenu.o dopreviousmenu.o redomenu.o recovermenu.o recoverallmenus.o\
          gotofirstmenu.o doicons.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
