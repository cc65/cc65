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

OBJS=	_scrsize.o	\
	break.o		\
	cclear.o	\
	cgetc.o		\
	chline.o	\
	clrscr.o	\
	color.o		\
	cputc.o		\
	crt0.o		\
	ctype.o		\
	cvline.o	\
        get_ostype.o    \
        getenv.o        \
        joy_stddrv.o    \
	kbhit.o		\
        mainargs.o      \
        randomize.o     \
	read.o		\
	revers.o	\
        systime.o       \
        sysuname.o      \
	where.o		\
	write.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = apple2-stdjoy.joy

TGIS =

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(OBJS) $(EMDS) $(JOYS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(TGIS)

