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
                cclear.o        \
                chline.o        \
                clock.o         \
                clrscr.o        \
                color.o         \
                cputc.o         \
                crt0.o          \
                ctype.o         \
                cvline.o        \
                getenv.o        \
                gotox.o         \
                gotoxy.o        \
                gotoy.o         \
                mainargs.o      \
                ppu.o           \
                ppubuf.o        \
                randomize.o     \
                revers.o        \
                setcursor.o     \
                sysuname.o      \
                waitvblank.o    \
                wherex.o        \
                wherey.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

