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
                cgetc.o         \
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
                kbhit.o         \
                mainargs.o      \
                revers.o        \
                systime.o       \
                sysuname.o      \
                wherex.o        \
                wherey.o        \
                write.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f $(C_OBJS:.c=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

