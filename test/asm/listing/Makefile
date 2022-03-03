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
	$(CA65) -t none -o $$(@:.bin=.o) $$<
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib

ifneq ($(wildcard $1.bin-ref),)
	$(ISEQUAL) $1.bin-ref $$@
endif

	$(CA65) -t none -l $$(@:.bin=.list.orig) -o $$(@:.bin=.list-o) $$<
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib

#	check if the result bin is the same as without listing file
	$(ISEQUAL) $$@ $$(@:.bin=.list-bin)

ifneq ($(wildcard $1.list-ref),)
#	we have a reference file, compare that, too

#	remove first line which contains a version number
	tail -n +2 $$(@:.bin=.lst.orig) > $$(@:.bin=.lst)
	$(ISEQUAL) $1.list-ref $$(@:.bin=.lst)
endif

#	$(CA65) -t none -f -l $$(@:.bin=.flist.orig) -o $$(@:.bin=.flist-o) $$<
#	$(LD65) -t none -o $$(@:.bin=.flist-bin) $$(@:.bin=.flist-o) none.lib

#	# check if the result bin is the same as without listing file
#	$(ISEQUAL) $$@ $$(@:.bin=.flist-bin)

endef # LISTING_template


$(foreach listing,$(LISTING_TESTS),$(eval $(call LISTING_template,$(listing))))


$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
