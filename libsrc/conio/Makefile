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

S_OBJS = _cursor.o	\
	 _scrsize.o	\
	 cputhex.o	\
	 cputs.o	\
	 cursor.o	\
	 scrsize.o

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)
