# Makefile for the assembler regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  DEL = del /f $(subst /,\,$1)
else
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  DEL = $(RM) $1
endif

ifdef QUIET
  .SILENT:
endif

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)

WORKDIR = ../../testwrk/asm

DIFF = $(WORKDIR)/bdiff$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

OPCODE_REFS := $(wildcard *-opcodes.ref)
OPCODE_CPUS = $(foreach ref,$(OPCODE_REFS),$(ref:%-opcodes.ref=%))
OPCODE_BINS = $(foreach cpu,$(OPCODE_CPUS),$(WORKDIR)/$(cpu)-opcodes.bin)

CPUDETECT_REFS := $(wildcard *-cpudetect.ref)
CPUDETECT_CPUS = $(foreach ref,$(CPUDETECT_REFS),$(ref:%-cpudetect.ref=%))
CPUDETECT_BINS = $(foreach cpu,$(CPUDETECT_CPUS),$(WORKDIR)/$(cpu)-cpudetect.bin)

all: $(OPCODE_BINS) $(CPUDETECT_BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(DIFF): ../bdiff.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define OPCODE_template

$(WORKDIR)/$1-opcodes.bin: $1-opcodes.s $(DIFF)
	$(if $(QUIET),echo asm/$1-opcodes.bin)
	$(CL65) --cpu $1 -t none -l $(WORKDIR)/$1-opcodes.lst -o $$@ $$<
	$(DIFF) $$@ $1-opcodes.ref

endef # OPCODE_template

$(foreach cpu,$(OPCODE_CPUS),$(eval $(call OPCODE_template,$(cpu))))

define CPUDETECT_template

$(WORKDIR)/$1-cpudetect.bin: cpudetect.s $(DIFF)
	$(if $(QUIET),echo asm/$1-cpudetect.bin)
	$(CL65) --cpu $1 -t none -l $(WORKDIR)/$1-cpudetect.lst -o $$@ $$<
	$(DIFF) $$@ $1-cpudetect.ref

endef # CPUDETECT_template

$(foreach cpu,$(CPUDETECT_CPUS),$(eval $(call CPUDETECT_template,$(cpu))))

clean:
	@$(call RMDIR,$(WORKDIR))
	@$(call DEL,$(OPCODE_REFS:.ref=.o) cpudetect.o)
