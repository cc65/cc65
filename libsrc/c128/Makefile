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
	break.o	       	\
     	cgetc.o	       	\
       	clrscr.o       	\
      	conio.o	       	\
     	crt0.o	       	\
       	color.o	       	\
     	cputc.o	       	\
     	kbhit.o	       	\
	mouse.o		\
	readjoy.o	\
	write.o

all:   	$(OBJS)

clean:
	@rm -f $(OBJS)

