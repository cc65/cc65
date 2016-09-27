
# makefile for the disassembler regression tests

BINDIR = ../../bin
WORKDIR := ../../testwrk

#BASE_TARGETS  = 6502 6502x 65sc02 65c02
#BASE_TARGETS += 4510 huc6280
BASE_TARGETS  = 4510

START = --start-addr 0x8000

DISASS_TARGETS = $(BASE_TARGETS)

# default target defined later
all:

# generate opcode targets and expand target list
define disass
DISASS_TARGETLIST += $$(WORKDIR)/$(1)-reass.bin $$(WORKDIR)/$(1)-reass.s $$(WORKDIR)/$(1)-disass.bin

$$(WORKDIR)/$(1)-disass.bin: $(1)-disass.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none $(START) --obj-path $$(WORKDIR) -o $$@ $$<
	@rm -f $(1)-disass.o #workaround for #168

$$(WORKDIR)/$(1)-reass.s: $$(WORKDIR)/$(1)-disass.bin
	@$$(BINDIR)/da65 --cpu $(1) $(START) -o $$@ $$<

$$(WORKDIR)/$(1)-reass.bin: $$(WORKDIR)/$(1)-reass.s
	@$$(BINDIR)/cl65 --cpu $(1) -t none $(START) --obj-path $$(WORKDIR) -o $$@ $$<
	@rm -f $(1)-reass.o #workaround for #168
	@cmp $$@ $$(WORKDIR)/$(1)-disass.bin
	@echo da65 --cpu $(1) ok
endef
$(foreach target,$(DISASS_TARGETS),$(eval $(call disass,$(target))))

# now that all targets have been generated, get to the manual ones
all: $(DISASS_TARGETLIST)
	@#

.PHONY: all $(DISASS_TARGETLIST)

