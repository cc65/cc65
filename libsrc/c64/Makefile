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

%.joy:	%.o ../runtime/zeropage.o
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
        get_tv.o                \
	joy_stddrv.o		\
       	kbhit.o	       	 	\
        kernal.o                \
       	mouse.o	       	 	\
        randomize.o             \
        revers.o                \
       	rs232.o	       	 	\
        tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS = c64-georam.emd c64-ram.emd c64-ramcart.emd c64-reu.emd c64-vdc.emd

JOYS = c64-stdjoy.joy

TGIS = c64-320-200-2.tgi

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

