#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

%.emd:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

%.tgi:	%.o ../runtime/zeropage.o
	@$(LD) -t module -o $@ $^

#--------------------------------------------------------------------------
# Object files

OBJS =	_scrsize.o	\
	break.o	       	\
     	cgetc.o	       	\
       	clrscr.o       	\
      	conio.o	       	\
     	crt0.o	       	\
       	color.o	       	\
     	cputc.o	       	\
     	kbhit.o	       	\
        kernal.o        \
	mouse.o		\
        randomize.o     \
	readjoy.o	\
	rs232.o		\
	tgi_mode_table.o

#--------------------------------------------------------------------------
# Drivers

TGIS =

EMDS =	c128-georam.emd c128-reu.emd

#--------------------------------------------------------------------------
# Targets

all:   	$(OBJS) $(EMDS) $(TGIS)

../runtime/zeropage.o:
	$(MAKE) -C $(dir $@) $(notdir $@)

clean:
	@rm -f $(OBJS)

