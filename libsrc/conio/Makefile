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

S_OBJS = cputs.o cursor.o cputhex.o scrsize.o

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)
