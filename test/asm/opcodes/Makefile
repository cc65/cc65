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

WORKDIR = ../../../testwrk/asm/opcodes

ISEQUAL = ../../../testwrk/isequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

OPCODE_REFS := $(wildcard *-opcodes.ref)
OPCODE_BINS = $(OPCODE_REFS:%.ref=$(WORKDIR)/%.bin)
OPCODE_CPUS = $(OPCODE_REFS:%-opcodes.ref=%)

all: $(OPCODE_BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define OPCODE_template

$(WORKDIR)/$1-opcodes.bin: $1-opcodes.s $(ISEQUAL)
	$(if $(QUIET),echo asm/$1-opcodes.bin)
	$(CA65) -t none --cpu $1 -l $$(@:.bin=.lst) -o $$(@:.bin=.o) $$<
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib
	$(ISEQUAL) $1-opcodes.ref $$@

endef # OPCODE_template

$(foreach cpu,$(OPCODE_CPUS),$(eval $(call OPCODE_template,$(cpu))))

$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
