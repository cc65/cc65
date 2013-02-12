#
# makefile for CC65 runtime library (C16)
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= c16

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
CO	= ../../src/co65/co65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I. -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

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

%-ser.o: %.ser
	@$(CO) -o $(*)-ser.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-ser.s

%-tgi.o: %.tgi
	@$(CO) -o $(*)-tgi.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-tgi.s

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o              \
        break.o                 \
        cgetc.o                 \
        clrscr.o                \
        color.o                 \
        conio.o                 \
        cputc.o                 \
        crt0.o                  \
        devnum.o                \
        get_tv.o                \
        irq.o                   \
        joy_stat_stddrv.o       \
        joy_stddrv.o            \
        kbhit.o                 \
        kclose.o                \
        kernal.o                \
        mainargs.o              \
        randomize.o             \
        revers.o                \
        status.o                \
        systime.o               \
        sysuname.o              \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)


#--------------------------------------------------------------------------
# Drivers

EMDS = c16-ram.emd

JOYS = c16-stdjoy.joy

SERS =

TGIS =

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS) $(EMDS) $(JOYS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) \
	       $(EMDS:.emd=.o) $(EMDS:.emd=-emd.s) \
	       $(JOYS:.joy=.o) $(JOYS:.joy=-joy.s) \
	       $(SERS:.ser=.o) $(SERS:.ser=-ser.s) \
	       $(TGIS:.tgi=.o) $(TGIS:.tgi=-tgi.s)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(SERS) $(TGIS)
