#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS  =       ctype.o                 \
                callroutine.o           \
                enterdesktop.o          \
                firstinit.o             \
                get_ostype.o            \
                getrandom.o             \
                getserialnumber.o       \
	        initdoneio.o            \
                mainloop.o              \
                panic.o                 \
                tobasic.o               \
                setdevice.o             \
                systime.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
