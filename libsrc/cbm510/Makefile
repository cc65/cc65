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
       	kbhit.o	 	\
       	kirq.o 	 	\
       	kplot.o	 	\
	kscnkey.o	\
	kudtim.o 	\
	mouse.o		\
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	readjoy.o	\
	revers.o	\
	rs232.o		\
	tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

TGIS =

EMDS =	cbm510-ram.emd

#--------------------------------------------------------------------------
# Targets

all:	$(OBJS) $(EMDS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(TGIS:.tgi=.o)

