#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= apple2

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.mou:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

C_OBJS=	closedir.o	\
	opendir.o	\
	readdir.o	\
	rewinddir.o

S_OBJS=	_scrsize.o	\
     	break.o		\
     	cclear.o	\
     	cgetc.o		\
     	chline.o	\
	close.o		\
     	clrscr.o	\
     	color.o		\
        cout.o          \
     	cputc.o		\
     	crt0.o		\
     	ctype.o		\
     	cvline.o	\
	dioclose.o	\
	diocommon.o	\
	dioopen.o	\
	dioread.o	\
	diosectcount.o	\
	diosectsize.o	\
	diowrite.o	\
	dosdetect.o	\
	filedes.o	\
	fileerr.o	\
	filename.o	\
        get_ostype.o    \
	gotoxy.o	\
	gotoy.o		\
        home.o          \
	initcwd.o	\
	iobuf.o         \
        joy_stddrv.o    \
     	kbhit.o		\
        mainargs.o      \
	mcbdefault.o	\
	mli.o		\
        mouse_stddrv.o  \
	open.o		\
        oserrlist.o     \
	oserror.o	\
        randomize.o     \
        rdkey.o         \
     	read.o          \
     	reboot.o        \
     	revers.o	\
	rwcommon.o	\
	syschdir.o	\
	sysmkdir.o	\
	sysremove.o	\
	sysrename.o	\
	sysrmdir.o	\
        systime.o       \
        sysuname.o      \
        tgi_mode_table.o\
        vtabz.o         \
     	wherex.o       	\
       	wherey.o        \
	write.o

S_EXTRA_OBJS=			\
	extra/iobuf-0800.o

#--------------------------------------------------------------------------
# Drivers

EMDS = apple2-auxmem.emd

JOYS = apple2-stdjoy.joy

MOUS = apple2-stdmou.mou

SERS = apple2-ssc.ser

TGIS = apple2-280-192-8.tgi apple2-40-48-16.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(C_OBJS) $(S_OBJS) $(S_EXTRA_OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS) $(S_EXTRA_OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:   	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)
