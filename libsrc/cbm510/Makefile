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

%.joy:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS =	_scrsize.o	\
	banking.o	\
       	break.o		\
       	cgetc.o		\
	clrscr.o	\
       	color.o	 	\
	conio.o		\
	cputc.o		\
       	crt0.o 	 	\
        extzp.o         \
       	kbhit.o	 	\
       	kirq.o 	 	\
       	kplot.o	 	\
	kscnkey.o	\
	kudtim.o 	\
        mainargs.o      \
	mouse.o		\
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	revers.o	\
	rs232.o		\
	tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS =	cbm510-ram.emd

JOYS = cbm510-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

zap:	clean
	@rm -f $(EMDS) $(JOYS) $(TGIS)

