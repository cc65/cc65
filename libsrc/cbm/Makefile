#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =  	cbm_load.o	\
		cbm_save.o 	\
		cbm_write.o

S_OBJS =	c_acptr.o	\
	 	c_basin.o 	\
		c_bsout.o	\
		c_chkin.o	\
	 	c_ciout.o	\
		c_ckout.o 	\
		c_clall.o	\
		c_close.o 	\
		c_clrch.o 	\
		c_iobase.o	\
		c_listen.o	\
		c_load.o	\
		c_open.o 	\
		c_readst.o 	\
		c_save.o	\
		c_setlfs.o	\
		c_setnam.o	\
		c_talk.o	\
		c_unlsn.o	\
		c_untlk.o	\
		cbm_close.o	\
		cbm_open.o 	\
		cbm_read.o 	\
		cclear.o 	\
		chline.o 	\
	 	clock.o 	\
		ctype.o 	\
		cvline.o 	\
		getenv.o 	\
		gotox.o 	\
		gotoxy.o 	\
		gotoy.o 	\
                oserrlist.o     \
		oserror.o	\
		revers.o	\
                systime.o       \
		where.o

all:	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~ *.bck $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

