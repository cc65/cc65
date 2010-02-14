#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= c16

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
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

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o	\
	break.o		\
       	cgetc.o	       	\
       	clrscr.o	\
       	color.o		\
       	conio.o		\
       	cputc.o		\
       	crt0.o		\
       	devnum.o	\
        get_tv.o        \
	joy_stddrv.o	\
        kacptr.o        \
        kbasin.o        \
       	kbhit.o		\
        kbsout.o        \
        kchkin.o        \
        kciout.o        \
        kckout.o        \
        kclall.o        \
        kclose.o        \
        kclrch.o        \
        kiobase.o       \
        klisten.o       \
        kload.o         \
        kopen.o         \
        kplot.o         \
        krdtim.o        \
        kreadst.o       \
        ksave.o         \
        ksetlfs.o       \
        ksetnam.o       \
        ksettim.o       \
        ktalk.o         \
        kunlsn.o        \
        kuntlk.o        \
        mainargs.o      \
        randomize.o     \
        revers.o        \
        systime.o       \
        sysuname.o      \
	tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = plus4-stdjoy.joy

SERS = plus4-stdser.ser

TGIS =

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(OBJS) $(EMDS) $(JOYS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(SERS) $(TGIS)

