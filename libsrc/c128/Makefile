#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= c128

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

OBJS =	_scrsize.o              \
        break.o                 \
        c64mode.o               \
        cgetc.o                 \
        clrscr.o                \
        conio.o                 \
        crt0.o                  \
        color.o                 \
        cputc.o                 \
        devnum.o                \
        fast.o                  \
        get_tv.o                \
        irq.o                   \
        joy_stat_stddrv.o       \
        joy_stddrv.o            \
        kbhit.o                 \
        kernal.o                \
        mainargs.o              \
        mcbdefault.o            \
        mouse_stat_stddrv.o     \
        mouse_stddrv.o          \
        randomize.o             \
        revers.o                \
        slow.o                  \
        status.o                \
        systime.o               \
        sysuname.o              \
        tgi_colors.o            \
        tgi_stat_stddrv.o       \
        tgi_stddrv.o            \
        toggle_videomode.o      \
        videomode.o             \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(MOUS:.mou=-mou.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)

#--------------------------------------------------------------------------
# Drivers

EMDS =  c128-georam.emd         \
        c128-ram.emd            \
        c128-ram2.emd           \
        c128-ramcart.emd        \
        c128-reu.emd            \
        c128-vdc.emd

JOYS =  c128-ptvjoy.joy         \
        c128-stdjoy.joy

MOUS =  c128-1351.mou           \
        c128-joymouse.mou       \
        c128-potmouse.mou

SERS =  c128-swlink.ser

TGIS =  c128-640-200-2.tgi      \
        c128-640-480-2.tgi

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
