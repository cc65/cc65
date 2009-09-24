#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= cbm610

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

%.emd:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.ser:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o	\
	banking.o	\
       	break.o		\
       	cgetc.o		\
	clrscr.o	\
       	color.o		\
       	cputc.o		\
       	crt0.o 		\
       	crtc.o 		\
        extzp.o         \
        get_tv.o        \
       	kbhit.o		\
	kclose.o	\
        kernal.o        \
        kiobase.o       \
        kopen.o         \
        krdtim.o        \
        kreadst.o       \
        kscnkey.o       \
        kscreen.o       \
        ksetnam.o       \
        ksettim.o       \
        kudtim.o        \
        mainargs.o      \
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	revers.o	\
        systime.o       \
        sysuname.o

#--------------------------------------------------------------------------
# Drivers

EMDS =	cbm610-ram.emd

JOYS =

SERS = cbm610-stdser.ser

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

