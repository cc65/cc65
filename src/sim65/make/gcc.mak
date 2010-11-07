#
# gcc Makefile for sim65
#

# ------------------------------------------------------------------------------

# The executable to build
EXE  	= sim65

# Library dir
COMMON	= ../common

#
CC	= gcc
CFLAGS 	= -g -O2 -Wall -W -std=c89
override CFLAGS += -I$(COMMON)
EBIND	= emxbind
LDFLAGS	= -ldl


# -----------------------------------------------------------------------------
# List of all object files

OBJS = 	addrspace.o     \
	callback.o      \
	cfgdata.o       \
	chip.o          \
	chippath.o      \
	config.o        \
	cpucore.o     	\
	cputype.o     	\
	error.o         \
	global.o      	\
	location.o      \
	main.o          \
	memory.o        \
	scanner.o       \
	system.o

LIBS = $(COMMON)/common.a

# ------------------------------------------------------------------------------
# Makefile targets

# Main target - must be first
.PHONY: all
ifeq (.depend,$(wildcard .depend))
all:	$(EXE) chips
include .depend
else
all:	depend
	@$(MAKE) -f make/gcc.mak all
endif

$(EXE):	$(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) $^ -o $@
	@if [ $(OS2_SHELL) ] ;	then $(EBIND) $(EXE) ; fi

.PHONY:	chips
chips:
	@$(MAKE) -C chips -f make/gcc.mak

clean:
	$(RM) *~ core.* *.map

zap:	clean
	$(RM) *.o $(EXE) .depend

# ------------------------------------------------------------------------------
# Make the dependencies

.PHONY: depend dep
depend dep:	$(OBJS:.o=.c)
	@echo "Creating dependency information"
	$(CC) $(CFLAGS) -MM $^ > .depend


