# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# Makefile for the assembler regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /q /s $(subst /,\,$1)
else
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
ifndef CMD_EXE
  CATERR = 2> $(WORKDIR)/$$@.errlog || (cat $(WORKDIR)/$$@.errlog && false)
endif
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)

WORKDIR = ../../../testwrk/asm/o65file

O65CHECK = ../../../testwrk/o65check$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

O65FILE_REFS := $(wildcard *.o65-ref)
O65FILE_BINS = $(O65FILE_REFS:%.o65-ref=$(WORKDIR)/%.o65)

all: $(O65FILE_BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(O65CHECK): o65check.h

$(O65CHECK): o65check.c o65check_file.c o65check_reference.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(WORKDIR)/%.o65: %.s $(O65CHECK)
	$(if $(QUIET),echo o65file/$<)
	$(CA65) --no-utf8 -t none -o $(@:.o65=.o) $<
	$(LD65) --no-utf8 -C $(<:.s=.config) -o $@ $(@:.o65=.o) none.lib
	$(O65CHECK) $@ $(<:.s=.o65-ref)

clean:
	@$(call RMDIR,$(WORKDIR))
