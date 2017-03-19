# Makefile for the regression tests that return an error code on failure

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  DEL = del /f $(subst /,\,$1)
else
  S = /
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  DEL = $(RM) $1
endif

SIM65FLAGS = -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ../../testwrk/val

OPTIONS = g O Os Osi Osir Oi Oir Or

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS := $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

# FIXME: These tests fail when built with optimizations for the 65c02
TESTS := $(filter-out $(WORKDIR)/compare7.O%.65c02.prg,$(TESTS))
TESTS := $(filter-out $(WORKDIR)/compare8.O%.65c02.prg,$(TESTS))
TESTS := $(filter-out $(WORKDIR)/compare9.O%.65c02.prg,$(TESTS))
TESTS := $(filter-out $(WORKDIR)/compare10.O%.65c02.prg,$(TESTS))
TESTS := $(filter-out $(WORKDIR)/or1.O%.65c02.prg,$(TESTS))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

# Some files have "K & R"-style syntax.  Therefore, some forward
# function-declarations don't match the later function definitions.
# Those programs fail when fastcall is used; but, the cdecl calling convention
# tolerates those conflicts.  Therefore, make their functions default to cdecl.
#
$(WORKDIR)/cq4.%.prg  \
$(WORKDIR)/cq71.%.prg \
$(WORKDIR)/cq81.%.prg \
$(WORKDIR)/cq84.%.prg: CC65FLAGS += -Wc --all-cdecl

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c | $(WORKDIR)
	$(CL65) -t sim$2 $$(CC65FLAGS) -$1 -o $$@ $$<
	$(SIM65) $(SIM65FLAGS) $$@ >$(NULLDEV)

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
	@$(call DEL,$(SOURCES:.c=.o))
