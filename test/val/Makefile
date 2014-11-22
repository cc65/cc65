
CC65FLAGS = -t sim6502

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),../../bin/sim65,sim65)

RM := rm -f

.PHONY: all

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
	$(SIM65) $@

%.o.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.os.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.osi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.osir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.oi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.oir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

%.or.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $@

clean:
	@$(RM) *.o
	@$(RM) *.prg
