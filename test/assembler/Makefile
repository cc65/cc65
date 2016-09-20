
# makefile for the assembler regression tests

BINDIR = ../../bin
WORKDIR := ../../testwrk

BASE_TARGETS  = 6502 6502x 65sc02 65c02
BASE_TARGETS += 4510 huc6280

OPCODE_TARGETS = $(BASE_TARGETS)
CPUDETECT_TARGETS = $(BASE_TARGETS)

CPUDETECT_TARGETS += 65816

# default target defined later
all:

# generate opcode targets and expand target list
define opcode
OPCODE_TARGETLIST += $$(WORKDIR)/$(1)-opcodes.bin
$$(WORKDIR)/$(1)-opcodes.bin: $(1)-opcodes.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none -l $$(WORKDIR)/$(1)-opcodes.lst --obj-path $$(WORKDIR) -o $$@ $$<
	@diff -q $(1)-opcodes.ref $$@ || (cat $$(WORKDIR)/$(1)-opcodes.lst ; exit 1)
	@echo ca65 --cpu $(1) opcodes ok
	@rm -f $(1)-opcodes.o #workaround for #168
endef
$(foreach target,$(OPCODE_TARGETS),$(eval $(call opcode,$(target))))

# generate cpudetect targets and expand target list
define cpudetect
CPUDETECT_TARGETLIST += $$(WORKDIR)/$(1)-cpudetect.bin
$$(WORKDIR)/$(1)-cpudetect.bin: cpudetect.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none -l $$(WORKDIR)/$(1)-cpudetect.lst --obj-path $$(WORKDIR) -o $$@ $$<
	@diff -q $(1)-cpudetect.ref $$@ || (cat $$(WORKDIR)/$(1)-cpudetect.lst ; exit 1)
	@echo ca65 --cpu $(1) cpudetect ok
	@rm -f cpudetect.o #workaround for #168
endef
$(foreach target,$(CPUDETECT_TARGETS),$(eval $(call cpudetect,$(target))))

# now that all targets have been generated, get to the manual ones
all: $(OPCODE_TARGETLIST) $(CPUDETECT_TARGETLIST)
	@#

.PHONY: all $(OPCODE_TARGETLIST) $(CPUDETECT_TARGETLIST)

