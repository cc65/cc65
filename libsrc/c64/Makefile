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

OBJS = 	_scrsize.o     	 	\
       	break.o	       	 	\
       	crt0.o 	       	 	\
       	cgetc.o	       	 	\
       	clrscr.o       	 	\
       	color.o	       	 	\
       	conio.o	       	 	\
       	cputc.o	       	 	\
        get_tv.o                \
       	kbhit.o	       	 	\
        kernal.o                \
       	mouse.o	       	 	\
        randomize.o             \
       	readjoy.o      	 	\
        revers.o                \
       	rs232.o	       	 	\
        tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

TGIS =	c64-320-200-2.tgi

EMDS =	c64-georam.emd c64-ram.emd c64-reu.emd c64-vdc.emd

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(TGIS:.tgi=.o)

