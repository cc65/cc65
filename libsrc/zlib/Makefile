#
# Makefile for the zlib subdirectory of the cc65 runtime library
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

C_OBJS =       	uncompress.o


S_OBJS =       	adler32.o       \
                crc32.o         \
                inflatemem.o


#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

zap:	clean

