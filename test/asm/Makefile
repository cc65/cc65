# Makefile for the assembler regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

ifdef QUIET
  .SILENT:
endif

CA65 := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)

WORKDIR = ../../testwrk/asm

ISEQUAL = $(WORKDIR)/isequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

OPCODE_REFS := $(wildcard *-opcodes.ref)
OPCODE_CPUS = $(foreach ref,$(OPCODE_REFS),$(ref:%-opcodes.ref=%))
OPCODE_BINS = $(foreach cpu,$(OPCODE_CPUS),$(WORKDIR)/$(cpu)-opcodes.bin)

CPUDETECT_REFS := $(wildcard *-cpudetect.ref)
CPUDETECT_CPUS = $(foreach ref,$(CPUDETECT_REFS),$(ref:%-cpudetect.ref=%))
CPUDETECT_BINS = $(foreach cpu,$(CPUDETECT_CPUS),$(WORKDIR)/$(cpu)-cpudetect.bin)

all: $(OPCODE_BINS) $(CPUDETECT_BINS) $(WORKDIR)/paramcount.o

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define OPCODE_template

$(WORKDIR)/$1-opcodes.bin: $1-opcodes.s $(ISEQUAL)
	$(if $(QUIET),echo asm/$1-opcodes.bin)
	$(CA65) --cpu $1 -t none -l $(WORKDIR)/$1-opcodes.lst -o $(WORKDIR)/$1-opcodes.o $$<
	$(LD65) -t none -o $$@ $(WORKDIR)/$1-opcodes.o none.lib
	$(ISEQUAL) $$@ $1-opcodes.ref

endef # OPCODE_template

$(foreach cpu,$(OPCODE_CPUS),$(eval $(call OPCODE_template,$(cpu))))

define CPUDETECT_template

$(WORKDIR)/$1-cpudetect.bin: cpudetect.s $(ISEQUAL)
	$(if $(QUIET),echo asm/$1-cpudetect.bin)
	$(CA65) --cpu $1 -t none -l $(WORKDIR)/$1-cpudetect.lst -o $(WORKDIR)/$1-cpudetect.o $$<
	$(LD65) -t none -o $$@ $(WORKDIR)/$1-cpudetect.o none.lib
	$(ISEQUAL) $$@ $1-cpudetect.ref

endef # CPUDETECT_template

$(foreach cpu,$(CPUDETECT_CPUS),$(eval $(call CPUDETECT_template,$(cpu))))

$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $(WORKDIR)/$@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
