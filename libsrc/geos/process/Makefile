#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= processinitrestartenable.o processblock.o processfreeze.o sleep.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
