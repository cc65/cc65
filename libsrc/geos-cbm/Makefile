#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= geos-cbm

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) --forget-inc-paths -I. -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) -o $(notdir $(*).s) $(CFLAGS) $<
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
# Directories

DIRS =	common		\
	conio		\
	devel		\
	disk		\
	file		\
	memory		\
	mousesprite	\
	process		\
	runtime		\
	system

#--------------------------------------------------------------------------
# Drivers

EMDS = geos-vdc.emd

JOYS = geos-stdjoy.joy

MOUS = #geos-stdmou.mou

SERS =

TGIS = geos-tgi.tgi

#--------------------------------------------------------------------------
# Directives

include $(addsuffix /Makefile, $(DIRS))
vpath %.c $(DIRS)
vpath %.s $(DIRS)

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all: 	$(C_OBJS) $(S_OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)

zap:	clean
	@$(RM) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)
