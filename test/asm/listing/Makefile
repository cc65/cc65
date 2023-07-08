# Makefile for the assembler regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /q /s $(subst /,\,$1)
  TRUE = exit 0
  CAT = type $(subst /,\,$1)
else
  S = /
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  TRUE = true
  CAT = cat $1
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
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) 2> $$(@:.bin=.err2)
ifeq ($(wildcard control/$1.no-ld65),)
ifeq ($(wildcard $1.cfg),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib > $$(@:.bin=.ld65-err) 2> $$(@:.bin=.ld65-err2)
else
	$(LD65) -C $$(<:.s=.cfg) -o $$@ $$(@:.bin=.o) > $$(@:.bin=.ld65-err) 2> $$(@:.bin=.ld65-err2)
endif
endif
else
	$(CA65) -t none -o $$(@:.bin=.o) $$< > $$(@:.bin=.err) 2> $$(@:.bin=.err2) || $(TRUE)
ifeq ($(wildcard control/$1.no-ld65),)
ifeq ($(wildcard $1.cfg),)
	$(LD65) -t none -o $$@ $$(@:.bin=.o) none.lib > $$(@:.bin=.ld65-err) 2> $$(@:.bin=.ld65-err2) || $(TRUE)
else
	$(LD65) -C $$(<:.s=.cfg) -o $$@ $$(@:.bin=.o) > $$(@:.bin=.ld65-err) 2> $$(@:.bin=.ld65-err2) || $(TRUE)
endif
endif
endif

ifneq ($(wildcard ref/$1.err-ref),)
	$(ISEQUAL) ref/$1.err-ref $$(@:.bin=.err)
else
	$(ISEQUAL) --empty $$(@:.bin=.err)
endif

ifneq ($(wildcard ref/$1.err2-ref),)
	$(ISEQUAL) ref/$1.err2-ref $$(@:.bin=.err2)
else
	$(ISEQUAL) --empty $$(@:.bin=.err2)
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

ifneq ($(wildcard ref/$1.ld65err2-ref),)
	@echo $(CAT) $$(@:.bin=.ld65-err2)
# FIXME: somehow this refuses to work in cmd.exe
ifndef CMD_EXE
	$(call CAT,$$(@:.bin=.ld65-err2))
	-diff -u ref/$1.ld65err2-ref $$(@:.bin=.ld65-err2)
endif
	$(ISEQUAL) --wildcards ref/$1.ld65err2-ref $$(@:.bin=.ld65-err2)
else
ifneq ($(wildcard $(WORKDIR)/$1.ld65-err2),)
	$(ISEQUAL) --empty $$(@:.bin=.ld65-err2)
endif
endif

#	compile with listing file
ifeq ($(wildcard control/$1.err),)
	$(CA65) -t none -l $$(@:.bin=.list-lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) 2> $$(@:.bin=.list-err2)
ifeq ($(wildcard control/$1.no-ld65),)
ifeq ($(wildcard $1.cfg),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.list-ld65-err) 2> $$(@:.bin=.list-ld65-err2)
else
	$(LD65) -C $$(<:.s=.cfg) -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) > $$(@:.bin=.list-ld65-err) 2> $$(@:.bin=.list-ld65-err2)
endif
endif
else
	$(CA65) -t none -l $$(@:.bin=.list-lst) -o $$(@:.bin=.list-o) $$< > $$(@:.bin=.list-err) 2> $$(@:.bin=.list-err2) || $(TRUE)
ifeq ($(wildcard control/$1.no-ld65),)
ifeq ($(wildcard $1.cfg),)
	$(LD65) -t none -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) none.lib > $$(@:.bin=.list-ld65-err) 2> $$(@:.bin=.list-ld65-err2) || $(TRUE)
else
	$(LD65) -C $$(<:.s=.cfg) -o $$(@:.bin=.list-bin) $$(@:.bin=.list-o) > $$(@:.bin=.list-ld65-err) 2> $$(@:.bin=.list-ld65-err2) || $(TRUE)
endif
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

ifneq ($(wildcard ref/$1.err2-ref),)
	$(ISEQUAL) ref/$1.err2-ref $$(@:.bin=.list-err2)
else
	$(ISEQUAL) --empty $$(@:.bin=.list-err2)
endif

ifneq ($(wildcard ref/$1.ld65err2-ref),)
	$(ISEQUAL) --wildcards ref/$1.ld65err2-ref $$(@:.bin=.list-ld65-err2)
else
ifneq ($(wildcard $(WORKDIR)/$1.list-ld65-err2),)
	$(ISEQUAL) --empty $$(@:.bin=.list-ld65-err2)
endif
endif

#	check if the result bin is the same as without listing file
ifeq ($(wildcard control/$1.err),)
ifeq ($(wildcard control/$1.err2),)
	$(ISEQUAL) $$@ $$(@:.bin=.list-bin)
endif
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
