# Makefile for the regression tests that return an error code on failure

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  S = /
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
endif

# sim65 can support 64-bit cycle counts on some platforms, but not all. This must fit in 32-bit.
SIM65FLAGS = -x 4000000000

CA65 := $(if $(wildcard ../../../bin/ca65*),..$S..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),..$S..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../../bin/sim65*),..$S..$S..$Sbin$Ssim65,sim65)

WORKDIR = ../../../testwrk/asm/val

.PHONY: all clean

SOURCES := $(wildcard *.s)
TESTS  = $(SOURCES:%.s=$(WORKDIR)/%.6502.prg)
TESTS += $(SOURCES:%.s=$(WORKDIR)/%.65c02.prg)

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

define PRG_template

$(WORKDIR)/%.$1.prg: %.s | $(WORKDIR)
	$(if $(QUIET),echo asm/val/$$*.$1.prg)
	$(CA65) -t sim$1 -o $$(@:.prg=.o) $$< $(NULLERR)
	$(LD65) -C sim6502-asmtest.cfg -o $$@ $$(@:.prg=.o) sim$1.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT)

endef # PRG_template

$(eval $(call PRG_template,6502))
$(eval $(call PRG_template,65c02))

clean:
	@$(call RMDIR,$(WORKDIR))
