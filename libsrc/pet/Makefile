#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@echo $<
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@echo $<
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =

S_OBJS = crt0.o kbhit.o conio.o clrscr.o cputc.o cgetc.o break.o color.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)
	@rm -f crt0.o

