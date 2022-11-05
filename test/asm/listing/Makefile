# Makefile for the assembler regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /q /s $(subst /,\,$1)
  ERRDIR = 2>&1
  TRUE = exit 0
  CAT = type $(subst /,\,$1)
else
  S = /
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  ERRDIR = 2>&1
  TRUE = true
  CAT = cat
endif

ifdef QUIET
#  .SILENT:
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),..$S..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),..$S..$S..$Sbin$Sld65,ld65)

WORKDIR = ../../../testwrk/asm/listing

ISEQUAL = ..$S..$S..$Stestwrk$Sisequal$(EXE)

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
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) $(ERRDIR)
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib > $$(@:.bin=.ld65-err) $(ERRDIR)
endif
else
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) $(ERRDIR) || $(TRUE)
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib > $$(@:.bin=.ld65-err) $(ERRDIR)  || $(TRUE)
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

#	rem $(indfo $(CAT) $(subst /,$$S,$$$(@:.bin=.ld65-err)))

ifneq ($(wildcard ref/$1.ld65err-ref),)
	@echo $(CAT) $$(@:.bin=.ld65-err)
# FIXME: somehow this refuses to work in cmd.exe
ifndef CMD_EXE
	$(call CAT,$$(@:.bin=.ld65-err))
	-diff -u ref/$1.ld65err-ref $$(@:.bin=.ld65-err)
endif
	$(ISEQUAL) --wildcards ref/$1.ld65err-ref $$(@:.bin=.ld65-err)
else
ifneq ($(wildcard $(WORKDIR)/$1.ld65-err),)
	$(ISEQUAL) --empty $$(@:.bin=.ld65-err)
endif
endif

#	compile with listing file
ifeq ($(wildcard control/$1.err),)
	$(CA65) -t none -l $$(@:.bin=.list-lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) $(ERRDIR)
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.list-ld65-err) $(ERRDIR)
endif
else
	$(CA65) -t none -l $$(@:.bin=.list-lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) $(ERRDIR) || $(TRUE)
ifeq ($(wildcard control/$1.no-ld65),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.list-ld65-err) $(ERRDIR) || $(TRUE)
endif
endif

ifneq ($(wildcard ref/$1.err-ref),)
	$(ISEQUAL) ref/$1.err-ref $$(@:.bin=.list-err)
else
	$(ISEQUAL) --empty $$(@:.bin=.list-err)
endif

ifneq ($(wildcard ref/$1.ld65err-ref),)
	$(ISEQUAL) --wildcards ref/$1.ld65err-ref $$(@:.bin=.list-ld65-err)
else
ifneq ($(wildcard $(WORKDIR)/$1.list-ld65-err),)
	$(ISEQUAL) --empty $$(@:.bin=.list-ld65-err)
endif
endif

#	check if the result bin is the same as without listing file
ifeq ($(wildcard control/$1.err),)
	$(ISEQUAL) $$@ $$(@:.bin=.list-bin)
endif

ifneq ($(wildcard ref/$1.list-ref),)
#	we have a reference file, compare that, too

#	remove first line which contains a version number
	$(ISEQUAL) --skip=1 ref/$1.list-ref $$(@:.bin=.list-lst)
endif

endef # LISTING_template


$(foreach listing,$(LISTING_TESTS),$(eval $(call LISTING_template,$(listing))))


$(WORKDIR)/%.o: %.s | $(WORKDIR)
	$(CA65) -l $(@:.o=.lst) -o $@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
