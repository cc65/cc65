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

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
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
       	kbhit.o		\
        kiec.o          \
        kiobase.o       \
       	kirq.o 		\
       	kplot.o		\
        krdtim.o        \
        kreadst.o       \
	kscnkey.o	\
        kscreen.o       \
        ksetlfs.o       \
        ksetnam.o       \
        ksettim.o       \
	kudtim.o	\
        kupdst.o        \
        mainargs.o      \
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	revers.o	\
	rs232.o         \
        sysuname.o

#--------------------------------------------------------------------------
# Drivers

TGIS =

EMDS =	cbm610-ram.emd

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS) $(EMDS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(TGIS)

