# -*- make -*-
#
# Makefile for GEOS lib
# for cc65
#

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%.emd:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.mou:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.ser:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

S_OBJS	= crt0.o                \
          extzp.o               \
          fio_module.o          \
          joy_stddrv.o          \
          mainargs.o            \
	  mcbdefault.o          \
          mouse_stddrv.o        \
          oserror.o             \
          oserrlist.o           \
          randomize.o           \
	  tgi_colors.o		\
          tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS = geos-vdc.emd

JOYS = geos-stdjoy.joy

MOUS = #geos-stdmou.mou

SERS =

TGIS = geos-tgi.tgi

all: $(S_OBJS) $(EMDS) $(JOYS) $(MOUS) $(SERS) $(TGIS)

../../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@$(RM) *.~ core $(S_OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(MOUS:.mou=.o) $(SERS:.ser=.o) $(TGIS:.tgi=.o)
