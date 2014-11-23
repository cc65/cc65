
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
DIFF := fc
else
RM := rm -f
DIFF := diff -q
endif

CFLAGS := -O2 -Wall -W -Wextra -fwrapv -fno-strict-overflow

.PHONY: all clean

REFS := $(patsubst %.c,%.ref,$(wildcard *.c))

TESTS := $(patsubst %.c,%.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.o.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.os.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.osi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.osir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.oi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.oir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.or.prg,$(wildcard *.c))

all: $(REFS) $(TESTS)

%.ref: %.c
	$(CC) $(CFLAGS) $< -o $*.host
	./$*.host > $@

%.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.o.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.os.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.osi.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.osir.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.oi.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.oir.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

%.or.prg: %.c %.ref
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out
	$(DIFF) $*.out $*.ref

clean:
	@$(RM) *.o
	@$(RM) *.prg
	@$(RM) *.out
	@$(RM) *.ref
	@$(RM) *.host
