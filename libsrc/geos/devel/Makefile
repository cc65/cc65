#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= crt0.o oserror.o randomize.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
