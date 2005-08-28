#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= pet

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

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
	cgetc.o		\
        checkst.o       \
	clrscr.o	\
	color.o		\
	conio.o		\
	cputc.o		\
	crt0.o		\
	get_tv.o	\
	kbhit.o         \
        kbsout.o        \
        kchkin.o        \
        kckout.o        \
        kclose.o        \
        kernal.o        \
        kopen.o         \
        kplot.o         \
        krdtim.o        \
        kreadst.o       \
        ksetlfs.o       \
        ksetnam.o       \
        mainargs.o      \
        randomize.o	\
	revers.o        \
        sysuname.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = pet-ptvjoy.joy

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


