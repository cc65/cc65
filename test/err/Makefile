
# makefile for the tests that MUST NOT compile

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  NOT := - # Hack
  DEL = del /f $(subst /,\,$1)
else
  NOT := !
  DEL = $(RM) $1
endif

CC65FLAGS := -t sim6502

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)

WORKDIR := ../../testwrk

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS := $(foreach option,. .o. .os. .osi. .osir. .oi. .oir. .or.,$(SOURCES:%.c=$(WORKDIR)/%$(option)prg))

all: $(TESTS)

$(WORKDIR)/%.prg: %.c
	$(NOT) $(CL65) $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.o.prg: %.c
	$(NOT) $(CL65) -O $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.os.prg: %.c
	$(NOT) $(CL65) -Os $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.osi.prg: %.c
	$(NOT) $(CL65) -Osi $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.osir.prg: %.c
	$(NOT) $(CL65) -Osir $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.oi.prg: %.c
	$(NOT) $(CL65) -Oi $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.oir.prg: %.c
	$(NOT) $(CL65) -Oir $(CC65FLAGS) $< -o $@
$(WORKDIR)/%.or.prg: %.c
	$(NOT) $(CL65) -Or $(CC65FLAGS) $< -o $@

clean:
	-@$(call DEL,$(TESTS))
	-@$(call DEL,$(SOURCES:.c=.o))
