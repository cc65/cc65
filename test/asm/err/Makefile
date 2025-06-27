# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# Makefile for the tests that MUST NOT compile

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  NOT = - # Hack
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  S = /
  NOT = !
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
ifndef CMD_EXE
  CATERR = 2> $(WORKDIR)/$$@.errlog || (cat $(WORKDIR)/$$@.errlog && false)
endif
endif

CA65 := $(if $(wildcard ../../../bin/ca65*),..$S..$S..$Sbin$Sca65,ca65)

WORKDIR = ../../../testwrk/asm/err

.PHONY: all clean

SOURCES := $(wildcard *.s)
TESTS = $(patsubst %.s,$(WORKDIR)/%.prg,$(SOURCES))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(WORKDIR)/%.prg: %.s | $(WORKDIR)
	$(if $(QUIET),echo asm/err/$*.s)
	$(NOT) $(CA65) -o $@ $< $(NULLERR)

clean:
	@$(call RMDIR,$(WORKDIR))
