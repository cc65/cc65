#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@echo $<
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= callroutine.o enterdesktop.o firstinit.o getrandom.o getserialnumber.o\
	  initdoneio.o mainloop.o panic.o tobasic.o setdevice.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core