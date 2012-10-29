#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= cbm610

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
	@$(CO) -o $(*)-emd.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-emd.s

%-joy.o: %.joy
	@$(CO) -o $(*)-joy.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-joy.s

%-ser.o: %.ser
	@$(CO) -o $(*)-ser.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-ser.s

%-tgi.o: %.tgi
	@$(CO) -o $(*)-tgi.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-tgi.s

%.emd:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o              \
        banking.o               \
        break.o                 \
        cgetc.o                 \
        clrscr.o                \
        color.o                 \
        cputc.o                 \
        crt0.o                  \
        crtc.o                  \
        devnum.o                \
        extzp.o                 \
        get_tv.o                \
        kbhit.o                 \
        kclose.o                \
        kernal.o                \
        kiobase.o               \
        kopen.o                 \
        krdtim.o                \
        kreadst.o               \
        kscnkey.o               \
        kscreen.o               \
        ksetnam.o               \
        ksettim.o               \
        kudtim.o                \
        mainargs.o              \
        peeksys.o               \
        pokesys.o               \
        randomize.o             \
        revers.o                \
        systime.o               \
        sysuname.o              \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)

#--------------------------------------------------------------------------
# Drivers

EMDS =	cbm610-ram.emd

JOYS =

SERS = cbm610-stdser.ser

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
