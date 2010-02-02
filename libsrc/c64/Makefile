#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c


#--------------------------------------------------------------------------
# Programs and flags

SYS	= c64

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

%.mou:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS = 	_scrsize.o     	 	\
       	break.o	       	 	\
       	crt0.o 	       	 	\
       	cgetc.o	       	 	\
       	clrscr.o       	 	\
       	color.o	       	 	\
       	conio.o	       	 	\
       	cputc.o	       	 	\
        get_ostype.o            \
        get_tv.o                \
	joy_stddrv.o		\
       	kbhit.o	       	 	\
        kernal.o                \
        kplot.o                 \
        mainargs.o              \
        mcbdefault.o            \
        mouse_stddrv.o          \
        randomize.o             \
        revers.o                \
        systime.o               \
        sysuname.o              \
        tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS =  c64-georam.emd          \
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

all: 	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

