#
# Makefile for the debug info test executable
#

# ------------------------------------------------------------------------------
# Generic stuff

# Environment variables for the watcom compiler
export WATCOM  = c:\\watcom
export INCLUDE = $(WATCOM)\\h

# We will use the windows compiler under linux (define as empty for windows)
export WINEDEBUG=fixme-all
WINE = wine

# Programs
AR     	= $(WINE) wlib
CC      = $(WINE) wcc386
LD     	= $(WINE) wlink
WSTRIP	= $(WINE) wstrip -q

LNKCFG  = ld.tmp

# Program arguments
CFLAGS  = -d1 -obeilr -zp4 -5 -zq -w2

# Target files
EXE	= dbgtest.exe

# Create NT programs by default
ifndef TARGET
TARGET = NT
endif

# --------------------- OS2 ---------------------
ifeq ($(TARGET),OS2)
SYSTEM  = os2v2
CFLAGS  += -bt=$(TARGET)
endif

# -------------------- DOS4G --------------------
ifeq ($(TARGET),DOS32)
SYSTEM  = dos4g
CFLAGS  += -bt=$(TARGET)
endif

# --------------------- NT ----------------------
ifeq ($(TARGET),NT)
SYSTEM  = nt
CFLAGS  += -bt=$(TARGET)
endif

# ------------------------------------------------------------------------------
# Implicit rules

%.obj:  %.c
	$(CC) $(CFLAGS) -fo=$@ $^


# ------------------------------------------------------------------------------
# Object files to link

OBJS = 	dbginfo.obj    	\
	dbgtest.obj  

# ------------------------------------------------------------------------------
# Main targets

all:	  	$(EXE)


# ------------------------------------------------------------------------------
# Other targets


$(EXE): 	$(OBJS)
	@echo "DEBUG ALL" > $(LNKCFG)
	@echo "OPTION QUIET" >> $(LNKCFG)
	@echo "OPTION MAP" >> $(LNKCFG)
	@echo "OPTION STACK=65536" >> $(LNKCFG)
	@echo "NAME $@" >> $(LNKCFG)
	@for i in $(OBJS); do echo "FILE $${i}"; done >> $(LNKCFG)
	@$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)

clean:
	@rm -f *~ core

zap:	clean
	@rm -f $(OBJS) $(EXE) $(EXE:.exe=.map)

strip:
	@-$(WSTRIP) $(EXE)

