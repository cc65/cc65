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
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)

WORKDIR = ../../../testwrk/asm/listing

ISEQUAL = ../../../testwrk/isequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

LISTING_SRC  := $(wildcard *.s)
LISTING_TESTS = $(LISTING_SRC:%.s=%)
LISTING_BINS  = $(LISTING_SRC:%.s=$(WORKDIR)/%.bin)

all: $(LISTING_BINS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<


define LISTING_template

$(WORKDIR)/$1.bin: $1.s $(ISEQUAL)
	$(if $(QUIET),echo asm/$1.bin)

#	compile without generating listing
ifeq ($(wildcard control/$1.err),)
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) 2>&1
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib
endif
else
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) 2>&1 || true
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib || true
endif
endif

ifneq ($(wildcard ref/$1.err-ref),)
	$(ISEQUAL) ref/$1.err-ref $$(@:.bin=.err)
else
	$(ISEQUAL) --empty $$(@:.bin=.err)
endif

ifneq ($(wildcard ref/$1.bin-ref),)
	$(ISEQUAL) --binary ref/$1.bin-ref $$@
endif

ifeq ($(wildcard control/$1.err),)
	$(CA65) -t none -l $$(@:.bin=.lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) 2>&1
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.ld65-err) 2>&1
endif
else
	$(CA65) -t none -l $$(@:.bin=.lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) 2>&1 || true
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.ld65-err) 2>&1 || true
endif
endif

ifneq ($(wildcard ref/$1.err-ref),)
	$(ISEQUAL) ref/$1.err-ref $$(@:.bin=.list-err)
else
	$(ISEQUAL) --empty $$(@:.bin=.list-err)
endif

ifneq ($(wildcard $(WORKDIR)/$1.ld65-err),)
ifneq ($(wildcard ref/$1.ld65err-ref),)
	$(ISEQUAL) ref/$1.ld65err-ref $$(@:.bin=.ld65-err)
else
	$(ISEQUAL) --empty $$(@:.bin=.ld65-err)
endif
endif

#	check if the result bin is the same as without listing file
ifeq ($(wildcard control/$1.err),)
	$(ISEQUAL) $$@ $$(@:.bin=.list-bin)
endif

ifneq ($(wildcard ref/$1.list-ref),)
#	we have a reference file, compare that, too

#	remove first line which contains a version number
	$(ISEQUAL) --skip=1 ref/$1.list-ref $$(@:.bin=.lst)
endif

endef # LISTING_template


$(foreach listing,$(LISTING_TESTS),$(eval $(call LISTING_template,$(listing))))


$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
