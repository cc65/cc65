# Makefile for the regression tests that generate output which has to be
# compared with reference output

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  DEL = del /f $(subst /,\,$1)
else
  S = /
  EXE =
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  DEL = $(RM) $1
endif

CC65FLAGS = -t sim6502
SIM65FLAGS = -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ..$S..$Stestwrk$Sref

OPTIONS = g O Os Osi Osir Oi Oir Or

DIFF = $(WORKDIR)$Sbdiff$(EXE)

CC = gcc
CFLAGS = -O2 -Wall -W -Wextra -funsigned-char -fwrapv -fno-strict-overflow

.PHONY: all clean

SOURCES := $(wildcard *.c)
REFS = $(SOURCES:%.c=$(WORKDIR)/%.ref)
TESTS = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).prg))

all: $(REFS) $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(WORKDIR)/%.ref: %.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $(WORKDIR)/$*.host $<
	$(WORKDIR)$S$*.host > $@

$(DIFF): ../bdiff.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

# Some files have "K & R"-style syntax.  Therefore, some forward
# function-declarations don't match the later function definitions.
# Those programs fail when fastcall is used; but, the cdecl calling convention
# tolerates those conflicts.  Therefore, make their functions default to cdecl.
#
$(WORKDIR)/init.%.prg   \
$(WORKDIR)/switch.%.prg \
$(WORKDIR)/yacc.%.prg   \
$(WORKDIR)/yaccdbg.%.prg: CC65FLAGS += -Wc --all-cdecl

# "yaccdbg.c" includes "yacc.c".
# yaccdbg's built files must depend on both of them.
#
$(WORKDIR)/yaccdbg.ref:   yacc.c
$(WORKDIR)/yaccdbg.%.prg: yacc.c

define PRG_template

$(WORKDIR)/%.$1.prg: %.c $(WORKDIR)/%.ref $(DIFF)
	$(CL65) $$(CC65FLAGS) -$1 -o $$@ $$<
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$*.out
	$(DIFF) $(WORKDIR)/$$*.out $(WORKDIR)/$$*.ref

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option))))

clean:
	@$(call RMDIR,$(WORKDIR))
	@$(call DEL,$(SOURCES:.c=.o))
