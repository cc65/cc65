#
# Makefile for GEOS lib
# for cc65
#
#

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<

C_OBJS	= messagebox.o
S_OBJS	= dodlgbox.o rstrfrmdialogue.o\
	  dbget2lines.o dlgboxyesno.o dlgboxokcancel.o dlgboxok.o dlgboxgetstring.o\
	  dlgboxfileselect.o

all: $(C_OBJS) $(S_OBJS)

clean:
	@rm -f core *.~ $(S_OBJS) $(C_OBJS:.o=.s) $(C_OBJS)
