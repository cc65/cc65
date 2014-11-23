
# makefile for the regression tests that return an error code on failure

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

.PHONY: all clean

TESTS := $(patsubst %.c,%.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.o.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.os.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.osi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.osir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.oi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.oir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,%.or.prg,$(wildcard *.c))

all: $(TESTS)

%.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.o.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.os.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.osi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.osir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.oi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.oir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

%.or.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@

clean:
	@$(RM) *.o
	@$(RM) *.prg
