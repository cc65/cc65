#
# Makefile for GEOS lib
# for cc65
#

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

C_OBJS	=
S_OBJS	= call.o

all: $(C_OBJS) $(S_OBJS)

clean:
	@rm -f core *.~ $(S_OBJS) $(C_OBJS:.o=.s) $(C_OBJS)
