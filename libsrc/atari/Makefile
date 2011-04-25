#
# makefile for CC65 Atari runtime library
#
# NUMDRVS - number of supported drives (max. 16)
#           4 bytes for each device are statically allocated
# LINEBUF - support line buffered reads from E: (the number specifies
#           the length of the buffer)
# UCASE_FILENAME - all filenames get uppercased
# DEFAULT_DEVICE - if the string passed to the uppercase function doesn't
#                  include a device (":" at position 2 or 3), provide "Dn:"
#                  as a default disk device
#                  n is the value of DEFAULT_DEVICE, unless DYNAMIC_DD is
#                  also set, in which case it's overridden by a runtime
#                  check (on SpartaDOS only)
#                  needs UCASE_FILENAME to be defined, otherwise no effect
# DYNAMIC_DD     - determine default disk device at runtime (SpartaDOS only)
#                  needs DEFAULT_DEVICE to be defined, otherwise no effect

ATARIDEFS =  -DNUMDRVS=4 -DUCASE_FILENAME=1 -DDEFAULT_DEVICE=1 -DLINEBUF=80
ATARIDEFS += -DDYNAMIC_DD=1

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= atari

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $(ATARIDEFS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $(ATARIDEFS) $<

%.emd: 	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o atari_tgi_common.inc
	@$(LD) -o $@ -t module $< ../runtime/zeropage.o


#--------------------------------------------------------------------------
# Object files

OBJS = 	_scrsize.o      \
	break.o         \
	cclear.o        \
	cgetc.o         \
	chline.o        \
	clock.o         \
	close.o         \
	clrscr.o        \
	color.o         \
	cputc.o         \
	crt0.o 	        \
	ctype.o         \
	cvline.o        \
	dio_cts.o       \
	dio_stc.o       \
	diopncls.o      \
	dioqsize.o      \
	dioread.o       \
	diowrite.o      \
	diowritev.o     \
	do_oserr.o      \
	dosdetect.o     \
	fdtable.o       \
	fdtab.o         \
	fdtoiocb.o      \
	getargs.o       \
	getdefdev.o     \
	getfd.o         \
	gotox.o         \
	gotoy.o         \
	gotoxy.o        \
	graphics.o      \
	initcwd.o       \
	inviocb.o       \
	joy_stddrv.o    \
	kbhit.o         \
	lseek.o         \
	mul40.o         \
	open.o 	        \
	oserrlist.o     \
	oserror.o       \
	ostype.o        \
	posixdirent.o   \
	randomize.o     \
	read.o 	        \
	revers.o        \
	rwcommon.o      \
	savevec.o       \
	scroll.o        \
	setcolor.o      \
	siocall.o       \
	syschdir.o      \
	sysmkdir.o      \
	sysremove.o     \
	sysrmdir.o      \
	systime.o       \
	sysuname.o      \
	tgi_colors.o    \
	tgi_mode_table.o\
	toascii.o       \
	tvtype.o        \
	ucase_fn.o      \
	wherex.o        \
	wherey.o        \
	write.o


#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = atari-stdjoy.joy atari-multijoy.joy

TGIS = atari-3.tgi atari-4.tgi atari-5.tgi atari-6.tgi atari-7.tgi atari-8.tgi \
       atari-9.tgi atari-10.tgi atari-11.tgi atari-14.tgi atari-15.tgi atari-8p2.tgi \
       atari-9p2.tgi atari-10p2.tgi atari-15p2.tgi

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(TGIS)

