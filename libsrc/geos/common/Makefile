#
# Makefile for GEOS lib
# for cc65
#

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

C_OBJS	= _afailed.o abort.o perror.o sleep.o
S_OBJS	= copydata.o memcpy.o memmove.o memset.o zerobss.o

all: $(C_OBJS) $(S_OBJS)

clean:
	@$(RM) core *.~ $(S_OBJS) $(C_OBJS:.o=.s) $(C_OBJS)
