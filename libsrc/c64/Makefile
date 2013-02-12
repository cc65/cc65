#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= c64

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
CO	= ../../src/co65/co65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
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

%-mou.o: %.mou
	@$(CO) -o $(*)-mou.s --code-label _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-mou.s

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

%.mou:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =  _scrsize.o              \
        break.o                 \
        crt0.o                  \
        cgetc.o                 \
        clrscr.o                \
        color.o                 \
        conio.o                 \
        cputc.o                 \
        devnum.o                \
        get_ostype.o            \
        get_tv.o                \
        irq.o                   \
        joy_stat_stddrv.o       \
        joy_stddrv.o            \
        kbhit.o                 \
        kernal.o                \
        kplot.o                 \
        mainargs.o              \
        mcbdefault.o            \
        mouse_stat_stddrv.o     \
        mouse_stddrv.o          \
        randomize.o             \
        revers.o                \
        status.o                \
        systime.o               \
        sysuname.o              \
        tgi_colors.o            \
        tgi_stat_stddrv.o       \
        tgi_stddrv.o            \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(MOUS:.mou=-mou.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)

#--------------------------------------------------------------------------
# Drivers

EMDS =  c64-c256k.emd           \
        c64-dqbb.emd            \
        c64-georam.emd          \
        c64-isepic.emd          \
        c64-ram.emd             \
        c64-ramcart.emd         \
        c64-reu.emd             \
        c64-vdc.emd             \
        dtv-himem.emd

JOYS =  c64-hitjoy.joy          \
        c64-numpad.joy          \
        c64-ptvjoy.joy          \
        c64-stdjoy.joy

MOUS =  c64-1351.mou            \
        c64-joymouse.mou        \
        c64-potmouse.mou

SERS =  c64-swlink.ser

TGIS =  c64-320-200-2.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) \
	       $(EMDS:.emd=.o) $(EMDS:.emd=-emd.s) \
	       $(JOYS:.joy=.o) $(JOYS:.joy=-joy.s) \
	       $(MOUS:.mou=.o) $(MOUS:.mou=-mou.s) \
	       $(SERS:.ser=.o) $(SERS:.ser=-ser.s) \
	       $(TGIS:.tgi=.o) $(TGIS:.tgi=-tgi.s)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)
