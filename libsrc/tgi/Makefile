#
# makefile for the TGI graphics kernel
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =

S_OBJS =	tgi-kernel.o	\
                tgi_geterror.o  \
                tgi_getmaxx.o   \
                tgi_getmaxy.o   \
                tgi_getxres.o   \
                tgi_getyres.o   \
		tgi_map_mode.o


all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

