#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

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
                sysuname.o

C_OBJS	=	systime.o

all: $(C_OBJS) $(S_OBJS)

clean:
	@rm -f *.~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS) core
