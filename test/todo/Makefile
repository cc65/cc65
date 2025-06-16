# Makefile for the currently failing regression tests that return an error code on failure

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
endif

SIM65FLAGS = -x 200000000

CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)
CA65 := $(if $(wildcard ../../bin/ca65*),..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ../../testwrk/todo

OPTIONS = g O Os Osi Osir Osr Oi Oir Or

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c | $(WORKDIR)
	$(if $(QUIET),echo todo/$$*.$1.$2.prg)
	$(CC65) -t sim$2 $$(CC65FLAGS) --add-source -$1 -o $$(@:.prg=.s) $$< $(NULLERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(NOT) $(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT)

endef # PRG_template

# HACK! for some reason the test does NOT fail in this particular config
$(WORKDIR)/inline-asm-1489.Osr.65c02.prg: inline-asm-1489.c | $(WORKDIR)
	$(if $(QUIET),echo todo/inline-asm-1489.Osr.65c02.prg)
	$(CC65) -t sim65c02 $$(CC65FLAGS) --add-source -Osr -o $(@:.prg=.s) $< $(NULLERR)
	$(CA65) -t sim65c02 -o $(@:.prg=.o) $(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim65c02 -o $@ $(@:.prg=.o) sim65c02.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $@ $(NULLOUT)

$(WORKDIR)/inline-asm-1489.Os.65c02.prg: inline-asm-1489.c | $(WORKDIR)
	$(if $(QUIET),echo todo/inline-asm-1489.Os.65c02.prg)
	$(CC65) -t sim65c02 $$(CC65FLAGS) --add-source -Os -o $(@:.prg=.s) $< $(NULLERR)
	$(CA65) -t sim65c02 -o $(@:.prg=.o) $(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim65c02 -o $@ $(@:.prg=.o) sim65c02.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $@ $(NULLOUT)

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
