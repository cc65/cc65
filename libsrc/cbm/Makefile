#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS = cbm_load.o cbm_save.o cbm_open.o cbm_read.o

S_OBJS = ctype.o getenv.o gotoxy.o gotox.o gotoy.o where.o\
	 clock.o chline.o cvline.o cclear.o revers.o\
	 c_readst.o c_close.o c_open.o c_ckout.o c_clrch.o c_bsout.o\
	 c_basin.o c_clall.o c_iobase.o c_setnam.o c_setlfs.o c_acptr.o\
	 c_ciout.o c_untlk.o c_unlsn.o c_listen.o c_talk.o c_load.o\
	 c_save.o oserror.o cbm_close.o c_chkin.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~ *.bck $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

