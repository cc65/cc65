#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

C_OBJS =

S_OBJS  =       _scrsize.o      \
                clock.o         \
                color.o         \
                crt0.o          \
                ppu.o           \
                ppubuf.o        \
                waitvblank.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

