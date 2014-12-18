
# makefile for the regression tests that return an error code on failure

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

CC65FLAGS := -t sim6502
SIM65FLAGS := -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),../../bin/sim65,sim65)

ifdef CMD_EXE
RM := del /f
else
RM := rm -f
endif

WORKDIR := ../../testwrk

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS := $(foreach option,. .o. .os. .osi. .osir. .oi. .oir. .or.,$(SOURCES:%.c=$(WORKDIR)/%$(option)prg))

all: $(TESTS)

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
	@$(RM) $(TESTS)
	@$(RM) $(SOURCES:.c=.o)
