#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS =	_scrsize.o	\
	banking.o	\
       	break.o		\
       	cgetc.o		\
	clrscr.o	\
       	color.o	 	\
	conio.o		\
	cputc.o		\
       	crt0.o 	 	\
       	kbhit.o	 	\
       	kirq.o 	 	\
       	kplot.o	 	\
	kscnkey.o	\
	kudtim.o 	\
	mouse.o		\
	peeksys.o	\
	pokesys.o	\
        randomize.o     \
	readjoy.o	\
	rs232.o		\
	tgi_mode_table.o

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

