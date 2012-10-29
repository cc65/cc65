#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= nes

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
CO	= ../../src/co65/co65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%-emd.o: %.emd
	@$(CO) -o $(*)-emd.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-emd.s

%-joy.o: %.joy
	@$(CO) -o $(*)-joy.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-joy.s

%-mou.o: %.mou
	@$(CO) -o $(*)-mou.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-mou.s

%-ser.o: %.ser
	@$(CO) -o $(*)-ser.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-ser.s

%-tgi.o: %.tgi
	@$(CO) -o $(*)-tgi.s --code-name _$(subst -,_,$(*)) $<
	@$(AS) -o $@ $(AFLAGS) $(*)-tgi.s

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

OBJS =  _scrsize.o              \
        cclear.o                \
        chline.o                \
        clock.o                 \
        clrscr.o                \
        color.o                 \
        cputc.o                 \
        crt0.o                  \
        ctype.o                 \
        cvline.o                \
        get_tv.o                \
        gotox.o                 \
        gotoxy.o                \
        gotoy.o                 \
        mainargs.o              \
        ppu.o                   \
        ppubuf.o                \
        randomize.o             \
        revers.o                \
        setcursor.o             \
        sysuname.o              \
        tgi_colors.o            \
        waitvblank.o            \
        wherex.o                \
        wherey.o                \
        $(EMDS:.emd=-emd.o)     \
        $(JOYS:.joy=-joy.o)     \
        $(MOUS:.mou=-mou.o)     \
        $(SERS:.ser=-ser.o)     \
        $(TGIS:.tgi=-tgi.o)

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

all:	$(OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/popa.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

nes-64-56-2.tgi:	nes-64-56-2.o		\
			../runtime/popa.o	\
			../runtime/zeropage.o	\
			clrscr.o		\
			cputc.o			\
			get_tv.o		\
			gotoxy.o		\
			ppu.o			\
			ppubuf.o		\
			setcursor.o
	@$(LD) -o $@ -t module $^

clean:
	@$(RM) $(OBJS) \
	       $(EMDS:.emd=.o) $(EMDS:.emd=-emd.s) \
	       $(JOYS:.joy=.o) $(JOYS:.joy=-joy.s) \
	       $(MOUS:.mou=.o) $(MOUS:.mou=-mou.s) \
	       $(SERS:.ser=.o) $(SERS:.ser=-ser.s) \
	       $(TGIS:.tgi=.o) $(TGIS:.tgi=-tgi.s)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)
