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
  NULLERR = 2>$(NULLDEV)
endif

CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)

WORKDIR = ../../testwrk/standard_err

OPTIONS = c89 c99 cc65

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c | $(WORKDIR)
	$(if $(QUIET),echo standard_err/$$*.$1.$2.prg)
	$(NOT) $(CC65) -t sim$2 $$(CC65FLAGS) -Osir --add-source --standard $1 -o $$(@:.prg=.s) $$< $(NULLERR)

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))

#$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
