# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# Makefile for the ca65 -E (expand only) regression test: the expanded source
# must be the same as the reference file, and it must assemble to the same
# code as the original source.

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /q /s $(subst /,\,$1)
  NULLDEV = nul:
else
  S = /
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  NULLDEV = /dev/null
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),..$S..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),..$S..$S..$Sbin$Sld65,ld65)

WORKDIR = ../../../testwrk/asm/expand

ISEQUAL = ..$S..$S..$Stestwrk$Sisequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

SOURCES := $(wildcard *.s)
TESTS    = $(SOURCES:%.s=$(WORKDIR)/%.bin)

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define EXPAND_template

$(WORKDIR)/$1.bin: $1.s ref/$1.i-ref $(ISEQUAL)
	$(if $(QUIET),echo asm/expand/$1.bin)

#	assemble the original source
	$(CA65) --no-utf8 -t none -o $$(@:.bin=.o) $$<
	$(LD65) --no-utf8 -t none -o $$@ $$(@:.bin=.o) none.lib

#	write the expanded source, there must not be an object file
	$(CA65) --no-utf8 -t none -E -o $(WORKDIR)/$1.i $$<
	$(ISEQUAL) ref/$1.i-ref $(WORKDIR)/$1.i $(NULLERR)

#	the expanded source must give the same code
	$(CA65) --no-utf8 -t none -o $$(@:.bin=.i.o) $(WORKDIR)/$1.i
	$(LD65) --no-utf8 -t none -o $$(@:.bin=.i.bin) $$(@:.bin=.i.o) none.lib
	$(ISEQUAL) --binary $$@ $$(@:.bin=.i.bin) $(NULLERR)

endef

$(foreach test,$(SOURCES:%.s=%),$(eval $(call EXPAND_template,$(test))))

clean:
	@$(call RMDIR,$(WORKDIR))
