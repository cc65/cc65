
# makefile for the assembler regression tests

BINDIR = ../../bin
#WORKDIR := ../../testwrk
WORKDIR := .

TARGETS  = 6502 6502x 65sc02 65c02
#TARGETS += 65816
TARGETS += 4510
TARGETS += huc6280
#TARGETS += m740

all: $(addprefix $(WORKDIR)/, $(addsuffix -opcodes.bin, $(TARGETS)))
	@#

.PHONY: all clean $(addprefix $(WORKDIR)/, $(addsuffix -opcodes.bin, $(TARGETS)))

clean:
	rm -f *.o *.bin *.lst

define build
$$(WORKDIR)/$(1)-opcodes.bin: $(1)-opcodes.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none -l $$(WORKDIR)/$(1)-opcodes.lst --obj-path $$(WORKDIR) -o $$@ $$<
	@diff -q $(1)-opcodes.ref $$@ || (cat $$(WORKDIR)/$(1)-opcodes.lst ; exit 1)
	@echo ca65 --cpu $(1) ok
endef

$(foreach target,$(TARGETS),$(eval $(call build,$(target))))

