#
# makefile for CC65 Atari runtime library
#
# NUMDRVS - number of supported drives (max. 16)
#           4 bytes for each device are statically allocated
# LINEBUF - support line buffered reads from E: (the number specifies
#           the length of the buffer)
# UCASE_FILENAME - all filenames get uppercased
# DEFAULT_DEVICE - if the string passed to the uppercase function doesn't
#                  include a device (":" at position 2 or 3), provide "D:"
#                  as a default device
#                  needs UCASE_FILENAME to be defined, otherwise no effect

ATARIDEFS = -DNUMDRVS=4 -DUCASE_FILENAME=1 -DDEFAULT_DEVICE=1 -DLINEBUF=80

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= atari

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $(ATARIDEFS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $(ATARIDEFS) $<

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

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
	dioread.o       \
	diowrite.o      \
	diowritev.o     \
	dosdetect.o     \
	fdtable.o       \
	getargs.o       \
	getdefdev.o     \
	getfd.o         \
	gotox.o         \
	gotoy.o         \
	gotoxy.o        \
	graphics.o      \
	graphuse.o      \
	joy_stddrv.o    \
	kbhit.o         \
	lseek.o         \
	mouse.o         \
	mul40.o         \
	open.o 	        \
	oserrlist.o     \
	oserror.o       \
	ostype.o        \
	randomize.o     \
	read.o 	        \
	revers.o        \
	rs232.o	        \
	rwcommon.o      \
	savevec.o       \
	scroll.o        \
	setcolor.o      \
	siocall.o       \
	sysremove.o     \
	systime.o       \
	tvtype.o        \
        sysuname.o      \
	ucase_fn.o      \
	wherex.o        \
	wherey.o        \
	write.o


#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = atari-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(TGIS)

