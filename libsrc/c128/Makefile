#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= c128

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

OBJS =	_scrsize.o		\
	break.o	       		\
	c64mode.o		\
     	cgetc.o	       		\
       	clrscr.o       		\
      	conio.o	       		\
     	crt0.o	       		\
       	color.o	       		\
     	cputc.o	       		\
	fast.o			\
        get_tv.o        	\
	joy_stddrv.o		\
     	kbhit.o	       		\
        kernal.o        	\
        mainargs.o              \
	mcbdefault.o		\
        randomize.o     	\
        revers.o        	\
	slow.o			\
        sysuname.o              \
	tgi_mode_table.o	\
	toggle_videomode.o

#--------------------------------------------------------------------------
# Drivers

EMDS = c128-georam.emd c128-ram.emd c128-ramcart.emd c128-reu.emd c128-vdc.emd

JOYS = c128-ptvjoy.joy c128-stdjoy.joy

MOUS = c128-1351.mou c128-joymouse.mou

SERS = c128-swlink.ser

TGIS = c128-640-200-2.tgi c128-640-480-2.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)


zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)


