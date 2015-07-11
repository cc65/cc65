
# makefile for the regression tests that generate output which has to be
# compared with reference output

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  S := $(subst /,\,/)
  DEL = -del /f $(subst /,\,$1)
else
  S := /
  DEL = $(RM) $1
endif

CC65FLAGS := -t sim6502
SIM65FLAGS := -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR := ..$S..$Stestwrk
DIFF := $(WORKDIR)/bdiff

CC := gcc
CFLAGS := -O2 -Wall -W -Wextra -fwrapv -fno-strict-overflow

.PHONY: all clean

SOURCES := $(wildcard *.c)
REFS := $(SOURCES:%.c=$(WORKDIR)/%.ref)
TESTS := $(foreach option,. .o. .os. .osi. .osir. .oi. .oir. .or.,$(SOURCES:%.c=$(WORKDIR)/%$(option)prg))

all: $(REFS) $(TESTS)

$(WORKDIR)/%.ref: %.c
	$(CC) $(CFLAGS) $< -o $(WORKDIR)/$*.host
	$(WORKDIR)$S$*.host > $@

# Some files have "K & R"-style syntax.  Therefore, some forward
# function-declarations don't match the later function definitions.
# Those programs fail when fastcall is used; but, the cdecl calling convention
# tolerates those conflicts.  Therefore, make their functions default to cdecl.
#
$(WORKDIR)/init%prg: CC65FLAGS += -Wc --all-cdecl
$(WORKDIR)/switch.%rg: CC65FLAGS += -Wc --all-cdecl
$(WORKDIR)/yacc.%rg: CC65FLAGS += -Wc --all-cdecl
$(WORKDIR)/yaccdbg%prg: CC65FLAGS += -Wc --all-cdecl

$(WORKDIR)/%.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.o.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -O $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.os.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Os $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.osi.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Osi $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.osir.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Osir $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.oi.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Oi $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.oir.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Oir $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.or.prg: %.c $(WORKDIR)/%.ref
	$(CL65) -Or $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

clean:
	@$(call DEL,$(TESTS))
	@$(call DEL,$(SOURCES:.c=.o))
	@$(call DEL,$(SOURCES:%.c=$(WORKDIR)/%.out))
	@$(call DEL,$(SOURCES:%.c=$(WORKDIR)/%.ref))
	@$(call DEL,$(SOURCES:%.c=$(WORKDIR)/%.host))
