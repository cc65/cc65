
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

WORKDIR := ./../../testwrk

.PHONY: all clean

TESTS := $(patsubst %.c,$(WORKDIR)/%.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.o.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.os.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.osi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.osir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.oi.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.oir.prg,$(wildcard *.c))
TESTS += $(patsubst %.c,$(WORKDIR)/%.or.prg,$(wildcard *.c))

all: $(TESTS)

$(WORKDIR)/%.prg: %.c
	! $(CL65) $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.o.prg: %.c
	! $(CL65) -O $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.os.prg: %.c
	! $(CL65) -Os $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.osi.prg: %.c
	! $(CL65) -Osi $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.osir.prg: %.c
	! $(CL65) -Osir $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.oi.prg: %.c
	! $(CL65) -Oi $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.oir.prg: %.c
	! $(CL65) -Oir $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.or.prg: %.c
	! $(CL65) -Or $(CC65FLAGS) $< -o $@
clean:
	@$(RM) $(TESTS)
	@$(RM) $(patsubst %.c,$(WORKDIR)/%.o,$(wildcard *.c))
