#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

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
       	kbhit.o		\
        kernal.o        \
        kiobase.o       \
        kopen.o         \
        krdtim.o        \
        kreadst.o       \
        kscreen.o       \
        ksetnam.o       \
        ksettim.o       \
        mainargs.o      \
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	revers.o	\
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

