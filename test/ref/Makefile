
# makefile for the regression tests that generate output which has to be
# compared with reference output

CC65FLAGS = -t sim6502
SIM65FLAGS = -x 200000000

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
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.o.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.os.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.osi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.osir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.oi.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.oir.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

%.or.prg: %.c
	$(CL65) $(CC65FLAGS) $< -o $@
	$(SIM65) $(SIM65FLAGS) $@ > $*.out

clean:
	@$(RM) *.o
	@$(RM) *.prg
	@$(RM) *.out
	@$(RM) *.ref
