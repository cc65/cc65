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


ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
  CATERR = 2> $(WORKDIR)/$$@.errlog || (cat $(WORKDIR)/$$@.errlog && false)
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)

WORKDIR = ../../../testwrk/asm/cpudetect

ISEQUAL = ../../../testwrk/isequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

CPUDETECT_REFS := $(wildcard *-cpudetect.ref)
CPUDETECT_BINS = $(CPUDETECT_REFS:%.ref=$(WORKDIR)/%.bin)
CPUDETECT_CPUS = $(CPUDETECT_REFS:%-cpudetect.ref=%)

all: $(CPUDETECT_BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define CPUDETECT_template

$(WORKDIR)/$1-cpudetect.bin: cpudetect.s $1-cpudetect.ref $(ISEQUAL)
	$(if $(QUIET),echo asm/$1-cpudetect.bin)
	$(CA65) -t none --cpu $1 -l $$(@:.bin=.lst) -o $$(@:.bin=.o) $$< $(CATERR)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib $(CATERR)
	$(ISEQUAL) $1-cpudetect.ref $$@

endef # CPUDETECT_template

$(foreach cpu,$(CPUDETECT_CPUS),$(eval $(call CPUDETECT_template,$(cpu))))

$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $@ $< $(NULLOUT) $(CATERR)

clean:
	@$(call RMDIR,$(WORKDIR))
