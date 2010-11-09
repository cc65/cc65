#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

%.emd:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.joy:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

%.tgi:	%.o ../../runtime/zeropage.o
	@$(LD) -o $@ -t module $^

S_OBJS	= crt0.o                \
          extzp.o               \
          fio_module.o          \
          joy_stddrv.o          \
          mainargs.o            \
          oserror.o             \
          oserrlist.o           \
          randomize.o           \
          tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

EMDS = geos-vdc.emd

JOYS = geos-stdjoy.joy

TGIS = geos-tgi.tgi

all: $(S_OBJS) $(EMDS) $(JOYS) $(TGIS)

clean:
	@$(RM) *.~ core $(S_OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)
