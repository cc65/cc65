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

EXELIST	=      	loader.system


# --------------------------------------------------------------------------
# Targets


.PHONY:	all
all:	$(EXELIST)


loader.system:	loader.s Makefile
	$(AS) -t apple2 --forget-inc-paths loader.s
	$(LD) -o $@ -C loader.cfg loader.o $(LIBDIR)/apple2.lib


# --------------------------------------------------------------------------
# Cleanup rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST)



