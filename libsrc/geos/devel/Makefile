#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

%.emd:	%.o ../../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.joy:	%.o ../../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

S_OBJS	= crt0.o                \
          extzp.o               \
          fio_module.o          \
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
	@rm -f *.~ core $(S_OBJS) $(EMDS:.emd=.o) $(JOYS:.joy=.o) $(TGIS:.tgi=.o)
