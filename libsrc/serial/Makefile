#
# Makefile for the serial driver API
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

C_OBJS =        ser_load.o

S_OBJS =       	ser-kernel.o	\
                ser_get.o       \
                ser_params.o    \
                ser_pause.o     \
                ser_put.o       \
                ser_status.o    \
		ser_unload.o    \
                ser_unpause.o


#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) *~ $(C_OBJS:.o=.s) $(C_OBJS) $(S_OBJS)

zap:	clean

