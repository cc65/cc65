#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =

S_OBJS  =       crt0.o          \
                systime.o

all:   	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS)

