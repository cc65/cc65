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
	break.o		\
	crt0.o		\
	cgetc.o		\
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	joy_stddrv.o	\
	kbhit.o		\
        kernal.o        \
        kplot.o         \
        randomize.o     \
	revers.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = vic20-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

