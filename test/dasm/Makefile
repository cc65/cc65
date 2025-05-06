# Makefile for the disassembler regression tests

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
endif

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
CA65 := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
DA65 := $(if $(wildcard ../../bin/da65*),../../bin/da65,da65)

WORKDIR = ../../testwrk/dasm

ISEQUAL = ../../testwrk/isequal$(EXE)

CC = gcc
CFLAGS = -O2

START = --start-addr 0x7000

.PHONY: all clean

SOURCES := $(wildcard *.s)
BINS = $(SOURCES:%disass.s=$(WORKDIR)/%reass.bin) $(WORKDIR)/65816-reass.bin
CPUS = $(SOURCES:%-disass.s=%)

all: $(BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define DISASS_template

$(WORKDIR)/$1-disass.bin: $1-disass.s | $(WORKDIR)
	$(CL65) --cpu $1 -t none $(START) -o $$@ $$<

$(WORKDIR)/$1-reass.s: $(WORKDIR)/$1-disass.bin
	$(DA65) --cpu $1 $(START) -o $$@ $$<

$(WORKDIR)/$1-reass.bin: $(WORKDIR)/$1-reass.s $(ISEQUAL)
	$(if $(QUIET),echo dasm/$1-reass.bin)
	$(CL65) --cpu $1 -t none $(START) -o $$@ $$<
	$(ISEQUAL) $(WORKDIR)/$1-disass.bin $$@

endef # DISASS_template

$(foreach cpu,$(CPUS),$(eval $(call DISASS_template,$(cpu))))

$(WORKDIR)/test65816.bin: test65816.s | $(WORKDIR)
	$(CA65) -o $(WORKDIR)/test65816.o $<
	$(LD65) -o $@ -C 65816.cfg $(WORKDIR)/test65816.o

$(WORKDIR)/65816-reass.s: $(WORKDIR)/test65816.bin
	$(DA65) --cpu 65816 -i 65816.info -o $@ $<

$(WORKDIR)/65816-reass.bin: $(WORKDIR)/65816-reass.s $(ISEQUAL)
	$(if $(QUIET),echo dasm/65816-reass.bin)
	$(CA65) -o $(WORKDIR)/65816-reass.o $<
	$(LD65) -o $@ -C 65816.cfg $(WORKDIR)/65816-reass.o
	$(ISEQUAL) --binary $(WORKDIR)/test65816.bin $@

clean:
	@$(call RMDIR,$(WORKDIR))
