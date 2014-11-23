
# makefile for the tests that MUST NOT compile

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

CC65FLAGS = -t sim6502

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)

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
	! $(CL65) $(CC65FLAGS) $< -o $@
%.o.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.os.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.osi.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.osir.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.oi.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.oir.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
%.or.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
clean:
	@$(RM) *.o
	@$(RM) *.prg
