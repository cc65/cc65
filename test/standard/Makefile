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

SIM65FLAGS = -x 4000000000 -c

CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)
CA65 := $(if $(wildcard ../../bin/ca65*),..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ../../testwrk/standard

OPTIONS = c89 c99 cc65

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
#TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c | $(WORKDIR)
	$(if $(QUIET),echo standard/$$*.$1.$2.prg)
	$(CC65) -t sim$2 $$(CC65FLAGS) -Osir --add-source --standard $1 -o $$(@:.prg=.s) $$< $(NULLERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$@.out

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))

#$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
