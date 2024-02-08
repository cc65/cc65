# Makefile for the remaining asm tests that need special care in one way or another

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  NOT = - # Hack
  EXE = .exe
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  S = /
  NOT = !
  EXE =
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

CA65 := $(if $(wildcard ../../../bin/ca65*),..$S..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../../bin/ld65*),..$S..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../../bin/sim65*),..$S..$S..$Sbin$Ssim65,sim65)

WORKDIR = ..$S..$S..$Stestwrk$Sasm$Smisc

.PHONY: all clean

SOURCES := $(wildcard *.s)
TESTS  = $(SOURCES:%.s=$(WORKDIR)/%.6502.prg)
TESTS += $(SOURCES:%.s=$(WORKDIR)/%.65c02.prg)

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

define PRG_template

# sim65 ensure 64-bit wait time does not timeout
$(WORKDIR)/sim65-timein.$1.prg: sim65-timein.s | $(WORKDIR)
	$(if $(QUIET),echo misc/sim65-timein.$1.prg)
	$(CA65) -t sim$1 -o $$(@:.prg=.o) $$< $(NULLERR)
	$(LD65) -t sim$1 -o $$@ $$(@:.prg=.o) sim$1.lib $(NULLERR)
	$(SIM65) -x 4400000000 -c $$@ $(NULLOUT) $(NULLERR)

# sim65 ensure 64-bit wait time does timeout
$(WORKDIR)/sim65-timeout.$1.prg: sim65-timeout.s | $(WORKDIR)
	$(if $(QUIET),echo misc/sim65-timeout.$1.prg)
	$(CA65) -t sim$1 -o $$(@:.prg=.o) $$< $(NULLERR)
	$(LD65) -t sim$1 -o $$@ $$(@:.prg=.o) sim$1.lib $(NULLERR)
	$(NOT) $(SIM65) -x 4400000000 -c $$@ $(NULLOUT) $(NULLERR)

endef # PRG_template

$(eval $(call PRG_template,6502))
$(eval $(call PRG_template,65c02))

clean:
	@$(call RMDIR,$(WORKDIR))
