#
# Makefile for the extended memory library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =        em_load.o

S_OBJS =       	em-kernel.o	\
		em_copyto.o	\
		em_copyfrom.o	\
		em_map.o	\
		em_mapclean.o 	\
		em_pagecount.o	\
		em_unload.o	\


all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)


