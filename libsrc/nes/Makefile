#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= nes

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
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.mou:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

#--------------------------------------------------------------------------
# Object files

OBJS =  _scrsize.o      \
        cclear.o        \
        chline.o        \
        clock.o         \
        clrscr.o        \
        color.o         \
        cputc.o         \
        crt0.o          \
        ctype.o         \
        cvline.o        \
        get_tv.o        \
        gotox.o         \
        gotoxy.o        \
        gotoy.o         \
        mainargs.o      \
        ppu.o           \
        ppubuf.o        \
        randomize.o     \
        revers.o        \
        setcursor.o     \
        sysuname.o      \
        waitvblank.o    \
        wherex.o        \
        wherey.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = nes-stdjoy.joy

MOUS =

SERS =

TGIS = nes-64-56-2.tgi

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/popa.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

nes-64-56-2.tgi:       	nes-64-56-2.o 		\
 			../runtime/popa.o     	\
 			../runtime/zeropage.o 	\
 			clrscr.o		\
 			cputc.o			\
 			get_tv.o		\
 			gotoxy.o		\
 			ppu.o 			\
 			ppubuf.o		\
 		       	setcursor.o
	@$(LD) -o $@ -t module $^

clean:
	@$(RM) $(OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)



