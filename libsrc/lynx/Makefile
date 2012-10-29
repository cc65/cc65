#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= lynx

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
CO	= ../../src/co65/co65
LD	= ../../src/ld65/ld65

AFLAGS 	= -t $(SYS) --forget-inc-paths -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
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

%-mou.o: %.mou
	@$(CO) -o $(*)-mou.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-mou.s

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

%.mou:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =  bllhdr.o                \
        bootldr.o               \
        cgetc.o                 \
        clock.o                 \
        crt0.o                  \
        ctype.o                 \
        defdir.o                \
        eeprom.o                \
        eeprom46.o              \
        eeprom66.o              \
        eeprom86.o              \
        exec.o                  \
        exehdr.o                \
        extzp.o                 \
        kbhit.o                 \
        load.o                  \
        lseek.o                 \
        lynx-cart.o             \
        mainargs.o              \
        open.o                  \
        oserror.o               \
        read.o                  \
        sysuname.o              \
        tgi_colors.o            \
        toascii.o               \
        uploader.o              \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(MOUS:.mou=-mou.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)


#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = lynx-stdjoy.joy

MOUS =

SERS = lynx-comlynx.ser

TGIS = lynx-160-102-16.tgi

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
