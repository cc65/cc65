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

S_OBJS = crt0.o kbhit.o conio.o clrscr.o cputc.o cgetc.o\
	 color.o break.o banking.o crtc.o pokesys.o\
	 kscnkey.o kplot.o kudtim.o kirq.o rs232.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS) crt0.o

