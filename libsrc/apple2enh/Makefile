#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= apple2enh

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../apple2 -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I ../apple2 -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	../apple2/%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	../apple2/%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS=	_scrsize.o	\
     	break.o		\
     	cclear.o	\
     	cgetc.o		\
     	chline.o	\
     	clrscr.o	\
     	color.o		\
     	cputc.o		\
     	crt0.o		\
     	ctype.o		\
     	cvline.o	\
	dosdetect.o	\
        get_ostype.o    \
        getenv.o        \
        joy_stddrv.o    \
     	kbhit.o		\
        mainargs.o      \
        oserrlist.o     \
        randomize.o     \
        rcout.o         \
     	read.o          \
     	revers.o	\
        rhome.o         \
        rom.o           \
        rpread.o        \
        rrdkey.o        \
        rvtabz.o        \
        systime.o       \
        sysuname.o      \
	textframe.o	\
        tgi_mode_table.o\
     	wherex.o       	\
       	wherey.o        \
	write.o

#--------------------------------------------------------------------------
# Drivers

EMDS = apple2-lc.emd

JOYS = apple2-stdjoy.joy

SERS =

TGIS = apple2-40-40-16.tgi apple2-280-192-6.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(OBJS) $(EMDS) $(JOYS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:   	clean
	@$(RM) $(EMDS) $(JOYS) $(SERS) $(TGIS)


