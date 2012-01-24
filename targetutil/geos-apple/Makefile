#
# Makefile
#

# --------------------------------------------------------------------------
# Programs
CC      = ../../src/cc65/cc65
AS      = ../../src/ca65/ca65
LD      = ../../src/ld65/ld65

# Directories
CFGDIR  = ../../src/ld65/cfg
INCDIR  = ../../include
LIBDIR  = ../../libsrc

# --------------------------------------------------------------------------
# List of executables.

EXELIST	=      	convert.system



# --------------------------------------------------------------------------
# Targets


.PHONY:	all
all:	$(EXELIST)


convert.system:	convert.c Makefile
	$(CC) --forget-inc-paths -I $(INCDIR) -t apple2enh -O convert.c
	$(AS) --forget-inc-paths convert.s
	$(LD) -o $@ -C $(CFGDIR)/apple2enh-system.cfg convert.o $(LIBDIR)/apple2enh.lib


# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST)



