#
# Makefile for the extended memory library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

C_OBJS =        em_load.o

S_OBJS =       	em-kernel.o	\
                em_commit.o     \
		em_copyto.o	\
		em_copyfrom.o	\
		em_map.o	\
		em_pagecount.o	\
		em_unload.o	\
                em_use.o


#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS) 

zap:	clean

