#
# Makefile for the TGI graphics kernel
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =        tgi_load.o

S_OBJS =       	tgi-kernel.o   	        \
                tgi_bar.o               \
                tgi_circle.o            \
                tgi_clear.o             \
                tgi_done.o              \
                tgi_emu_bar.o           \
                tgi_getcolor.o          \
                tgi_getcolorcount.o     \
                tgi_geterror.o          \
                tgi_getmaxcolor.o       \
                tgi_getmaxx.o           \
                tgi_getmaxy.o           \
                tgi_getpixel.o          \
                tgi_getset.o            \
                tgi_getxres.o           \
                tgi_getyres.o           \
                tgi_gotoxy.o            \
                tgi_init.o              \
                tgi_line.o              \
                tgi_linepop.o           \
                tgi_lineto.o            \
       	       	tgi_map_mode.o          \
                tgi_setcolor.o          \
                tgi_setdrawpage.o       \
                tgi_setpixel.o          \
                tgi_setdrawpage.o       \
                tgi_unload.o


all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

