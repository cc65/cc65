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

OBJS =	_scrsize.o	\
	break.o	       	\
     	cgetc.o	       	\
       	clrscr.o       	\
      	conio.o	       	\
     	crt0.o	       	\
       	color.o	       	\
     	cputc.o	       	\
        get_tv.o        \
     	kbhit.o	       	\
        kernal.o        \
	mouse.o		\
        randomize.o     \
        revers.o        \
	rs232.o		\
	tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS =	c128-georam.emd c128-ram.emd c128-reu.emd c128-vdc.emd

JOYS = c128-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

all:   	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

