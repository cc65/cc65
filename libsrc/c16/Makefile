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

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o   	\
	break.o	     	\
       	cgetc.o	       	\
	clrscr.o     	\
	color.o	     	\
	conio.o	     	\
	cputc.o	     	\
	crt0.o	     	\
        get_tv.o        \
	joy_stddrv.o	\
	kbhit.o	     	\
        kclose.o        \
        kernal.o        \
        mainargs.o      \
        randomize.o     \
        revers.o        \
        sysuname.o

#--------------------------------------------------------------------------
# Drivers

EMDS = c16-ram.emd

JOYS = c16-stdjoy.joy

SERS =

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

