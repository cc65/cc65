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

S_OBJS = ctype.o getenv.o gotoxy.o gotox.o gotoy.o where.o\
	 clock.o chline.o cvline.o cclear.o revers.o\
	 c_readst.o c_close.o c_open.o c_ckout.o c_clrch.o c_bsout.o\
	 c_basin.o c_clall.o c_iobase.o c_setnam.o c_setlfs.o c_acptr.o\
	 c_ciout.o c_untlk.o c_unlsn.o c_listen.o c_talk.o c_load.o\
	 oserror.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.c=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

