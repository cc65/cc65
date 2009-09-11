#
# Makefile for the TGI graphics kernel
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= none

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

C_OBJS =        tgi_load.o              \
                tgi_load_driver.o

S_OBJS =       	tgi-kernel.o   	        \
                tgi_bar.o               \
                tgi_circle.o            \
                tgi_clear.o             \
                tgi_curtoxy.o           \
                tgi_done.o              \
                tgi_getcolor.o          \
                tgi_getcolorcount.o     \
                tgi_getdefpalette.o     \
                tgi_geterror.o          \
                tgi_geterrormsg.o       \
                tgi_getmaxcolor.o       \
                tgi_getmaxx.o           \
                tgi_getmaxy.o           \
                tgi_getpagecount.o      \
                tgi_getpalette.o        \
                tgi_getpixel.o          \
                tgi_getset.o            \
                tgi_getxres.o           \
                tgi_getyres.o           \
                tgi_gotoxy.o            \
                tgi_init.o              \
                tgi_ioctl.o             \
                tgi_line.o              \
                tgi_linepop.o           \
                tgi_lineto.o            \
       	       	tgi_map_mode.o          \
                tgi_outtext.o           \
                tgi_outtextxy.o         \
                tgi_popxy.o             \
                tgi_popxy2.o            \
                tgi_setcolor.o          \
                tgi_setdrawpage.o       \
                tgi_setpalette.o        \
                tgi_setpixel.o          \
                tgi_setviewpage.o       \
                tgi_textsize.o          \
                tgi_textstyle.o         \
                tgi_unload.o


#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

