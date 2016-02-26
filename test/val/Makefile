
# makefile for the regression tests that return an error code on failure

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  DEL = -del /f $(subst /,\,$1)
else
  DEL = $(RM) $1
endif

CC65FLAGS := -t sim6502
SIM65FLAGS := -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),../../bin/sim65,sim65)

WORKDIR := ../../testwrk

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS := $(foreach option,. .o. .os. .osi. .osir. .oi. .oir. .or.,$(SOURCES:%.c=$(WORKDIR)/%$(option)prg))

all: $(TESTS)

# Some files have "K & R"-style syntax.  Therefore, some forward
# function-declarations don't match the later function definitions.
# Those programs fail when fastcall is used; but, the cdecl calling convention
# tolerates those conflicts.  Therefore, make their functions default to cdecl.
#
$(WORKDIR)/cq4%prg $(WORKDIR)/cq71.%rg $(WORKDIR)/cq81%prg $(WORKDIR)/cq84%prg: CC65FLAGS += -Wc --all-cdecl

$(WORKDIR)/%.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.o.prg: %.c
	$(CL65) -O $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.os.prg: %.c
	$(CL65) -Os $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.osi.prg: %.c
	$(CL65) -Osi $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.osir.prg: %.c
	$(CL65) -Osir $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.oi.prg: %.c
	$(CL65) -Oi $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.oir.prg: %.c
	$(CL65) -Oir $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

$(WORKDIR)/%.or.prg: %.c
	$(CL65) -Or $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

clean:
	@$(call DEL,$(TESTS))
	@$(call DEL,$(SOURCES:.c=.o))
