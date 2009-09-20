#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c


#--------------------------------------------------------------------------
# Programs and flags

SYS	= lynx

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

%.emd:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.joy:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.mou:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.ser:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o extzp.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS =  cgetc.o         \
        crt0.o          \
	ctype.o		\
        eeprom.o        \
        extzp.o         \
        kbhit.o         \
	mainargs.o	\
	sysuname.o	\
        upload.o

#--------------------------------------------------------------------------
# Drivers

EMDS =

JOYS = lynx-stdjoy.joy

MOUS =

SERS = lynx-comlynx.ser

TGIS = lynx-160-102-16.tgi

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

