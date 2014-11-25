
# makefile for the regression tests that generate output which has to be
# compared with reference output

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

CC65FLAGS = -t sim6502
SIM65FLAGS = -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),../../bin/sim65,sim65)

ifdef CMD_EXE
RM := del /f
else
RM := rm -f
endif

WORKDIR := ./../../testwrk

DIFF := $(WORKDIR)/bdiff

CFLAGS := -O2 -Wall -W -Wextra -fwrapv -fno-strict-overflow

.PHONY: all clean

REFS := $(patsubst %.c,$(WORKDIR)/%.ref,$(wildcard *.c))

TESTS := $(patsubst %.c,$(WORKDIR)/%.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.o.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.os.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.osi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.osir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.oi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.oir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.or.prg,$(wildcard *.c))

all: $(REFS) $(TESTS)

$(WORKDIR)/%.ref: %.c
	$(CC) $(CFLAGS) $< -o $(WORKDIR)/$*.host
	$(WORKDIR)/$*.host > $@

$(WORKDIR)/%.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.o.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.os.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.osi.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.osir.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.oi.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.oir.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

$(WORKDIR)/%.or.prg: %.c $(WORKDIR)/%.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $(WORKDIR)/$*.out
	$(DIFF) $(WORKDIR)/$*.out $(WORKDIR)/$*.ref

clean:
	@$(RM) $(TESTS)
	@$(RM) $(patsubst %.c,$(WORKDIR)/%.o,$(wildcard *.c))
	@$(RM) $(patsubst %.c,$(WORKDIR)/%.out,$(wildcard *.c))
	@$(RM) $(patsubst %.c,$(WORKDIR)/%.ref,$(wildcard *.c))
	@$(RM) $(patsubst %.c,$(WORKDIR)/%.host,$(wildcard *.c))
