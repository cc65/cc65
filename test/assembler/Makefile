
# makefile for the assembler regression tests

BINDIR = ../../bin

BASE_TARGETS  = 6502 6502x 65sc02 65c02
BASE_TARGETS += 4510 huc6280

OPCODE_TARGETS = $(BASE_TARGETS)
CPUDETECT_TARGETS = $(BASE_TARGETS)

CPUDETECT_TARGETS += 65816

# default target defined later
all:

# generate opcode targets and expand target list
define opcode
OPCODE_TARGETLIST += $(1)-opcodes.bin
$(1)-opcodes.bin: $(1)-opcodes.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none -l $(1)-opcodes.lst -o $$@ $$<
	@diff -q $(1)-opcodes.ref $$@ || (cat $(1)-opcodes.lst ; exit 1)
	@echo ca65 --cpu $(1) opcodes ok
endef
$(foreach target,$(OPCODE_TARGETS),$(eval $(call opcode,$(target))))

# generate cpudetect targets and expand target list
define cpudetect
CPUDETECT_TARGETLIST += $(1)-cpudetect.bin
$(1)-cpudetect.bin: cpudetect.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none -l $(1)-cpudetect.lst -o $$@ $$<
	@diff -q $(1)-cpudetect.ref $$@ || (cat $(1)-cpudetect.lst ; exit 1)
	@echo ca65 --cpu $(1) cpudetect ok
endef
$(foreach target,$(CPUDETECT_TARGETS),$(eval $(call cpudetect,$(target))))

# now that all targets have been generated, get to the manual ones
all: $(OPCODE_TARGETLIST) $(CPUDETECT_TARGETLIST)
	@#

clean:
	rm -f *.o *.bin *.lst

.PHONY: all clean $(OPCODE_TARGETLIST) $(CPUDETECT_TARGETLIST)

