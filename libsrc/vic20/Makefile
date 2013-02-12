#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= vic20

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
CO	= ../../src/co65/co65
LD	= ../../src/ld65/ld65

AFLAGS=-t $(SYS) --forget-inc-paths -I../../asminc
CFLAGS=-Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%-emd.o: %.emd
	@$(CO) -o $(*)-emd.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-emd.s

%-joy.o: %.joy
	@$(CO) -o $(*)-joy.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-joy.s

%-tgi.o: %.tgi
	@$(CO) -o $(*)-tgi.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-tgi.s

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o              \
        break.o                 \
        crt0.o                  \
        cgetc.o                 \
        clrscr.o                \
        color.o                 \
        conio.o                 \
        cputc.o                 \
        devnum.o                \
        get_tv.o                \
        irq.o                   \
        joy_stat_stddrv.o       \
        joy_stddrv.o            \
        kbhit.o                 \
        kernal.o                \
        kplot.o                 \
        mainargs.o              \
        randomize.o             \
        revers.o                \
        status.o                \
        sysuname.o              \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(TGIS:.tgi=-tgi.o)

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = vic20-stdjoy.joy vic20-ptvjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) \
	       $(EMDS:.emd=.o) $(EMDS:.emd=-emd.s) \
	       $(JOYS:.joy=.o) $(JOYS:.joy=-joy.s) \
	       $(TGIS:.tgi=.o) $(TGIS:.tgi=-tgi.s)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(TGIS)
