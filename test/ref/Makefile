# Makefile for the regression tests that generate output which has to be
# compared with reference output

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  DEL = del /f $(subst /,\,$1)
else
  S = /
  EXE =
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  DEL = $(RM) $1
endif

ifdef QUIET
  .SILENT:
  NULLERR = 2>$(NULLDEV)
endif

SIM65FLAGS = -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),..$S..$Sbin$Scl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ..$S..$Stestwrk$Sref

OPTIONS = g O Os Osi Osir Osr Oi Oir Or

DIFF = $(WORKDIR)$Sbdiff$(EXE)

CC = gcc
CFLAGS = -O2 -Wall -W -Wextra -funsigned-char -fwrapv -fno-strict-overflow

.PHONY: all clean

SOURCES := $(wildcard *.c)
REFS = $(SOURCES:%.c=$(WORKDIR)/%.ref)
TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

all: $(REFS) $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(WORKDIR)/%.ref: %.c | $(WORKDIR)
	$(if $(QUIET),echo ref/$*.host)
	$(CC) $(CFLAGS) -o $(WORKDIR)/$*.host $< $(NULLERR)
	$(WORKDIR)$S$*.host > $@

$(DIFF): ../bdiff.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

# "yaccdbg.c" includes "yacc.c".
# yaccdbg's built files must depend on both of them.
#
$(WORKDIR)/yaccdbg.ref:   yacc.c
$(WORKDIR)/yaccdbg.%.prg: yacc.c

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c $(WORKDIR)/%.ref $(DIFF)
	$(if $(QUIET),echo ref/$$*.$1.$2.prg)
	$(CL65) -t sim$2 $$(CC65FLAGS) -$1 -o $$@ $$< $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$*.out
	$(DIFF) $(WORKDIR)/$$*.out $(WORKDIR)/$$*.ref

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
	@$(call DEL,$(SOURCES:.c=.o))
