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
       	color.o	 	\
       	crt0.o 	 	\
       	kbhit.o	 	\
       	kirq.o 	 	\
       	kplot.o	 	\
	kscnkey.o	\
	kudtim.o 	\
	pokesys.o

all:	$(OBJS)

clean:
	@rm -f $(OBJS)

