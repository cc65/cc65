#
# makefile for CC65 runtime library (C16)
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
	break.o		\
       	cgetc.o	       	\
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	crt0.o		\
        get_tv.o        \
	kbhit.o		\
        kernal.o        \
        randomize.o     \
        revers.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = c16-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)


