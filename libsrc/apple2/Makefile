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

S_OBJS = break.o clrscr.o cclear.o cgetc.o chline.o color.o \
	 cputc.o crt0.o ctype.o \
	 cvline.o kbhit.o read.o revers.o where.o write.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)
	@rm -f crt0.o

