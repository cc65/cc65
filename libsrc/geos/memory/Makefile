#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@echo $<
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= crc.o doublepop.o reuregs.o clearram.o fillram.o initram.o movedata.o\
	  stashram.o fetchram.o swapram.o verifyram.o\
	  doublespop.o copystring.o cmpstring.o copyfstring.o cmpfstring.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core