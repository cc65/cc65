#
# Makefile for GEOS lib
# for cc65
#

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

C_OBJS	= _afailed.o abort.o perror.o
S_OBJS	= copydata.o memcpy.o memset.o rand.o

all: $(C_OBJS) $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
