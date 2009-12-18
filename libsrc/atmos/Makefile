#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= atmos

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
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

%.mou:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS  =	_scrsize.o      \
        cclear.o        \
        cgetc.o         \
        chline.o        \
        clock.o         \
        clrscr.o        \
        color.o         \
        cputc.o         \
        crt0.o          \
        ctype.o         \
        cvline.o        \
        gotox.o         \
        gotoxy.o        \
        gotoy.o         \
        kbhit.o         \
        mainargs.o      \
        oserrlist.o     \
        revers.o        \
        systime.o       \
        sysuname.o      \
        wherex.o        \
        wherey.o        \
        write.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS =

MOUS =

SERS =

TGIS = atmos-240-200-2.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)


