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

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
ifndef CMD_EXE
  CATERR = 2> $@.errlog && (cat $@.errlog && true)
endif
endif


CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)

WORKDIR = ../../testwrk/err

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS = $(patsubst %.c,$(WORKDIR)/%.s,$(SOURCES))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(WORKDIR)/%.s: %.c | $(WORKDIR)
	$(if $(QUIET),echo err/$*.s)
	$(NOT) $(CC65) --no-utf8 -o $@ $< $(NULLOUT) $(CATERR)

clean:
	@$(call RMDIR,$(WORKDIR))
