#
# Makefile for GEOS lib
# for cc65
#
#

%.o:	%.s
	@echo $<
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= dodlgbox.o rstrfrmdialogue.o\
	  dbget2lines.o dlgboxyesno.o dlgboxokcancel.o dlgboxok.o dlgboxgetstring.o\
	  dlgboxfileselect.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core