# makefile for the tests that MUST NOT compile

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  NOT := - # Hack
  DEL = -del /f $(subst /,\,$1)
else
  NOT := !
  DEL = $(RM) $1
endif

CC65 := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)

WORKDIR := ../../testwrk

.PHONY: all clean

TESTS := $(patsubst %.c,$(WORKDIR)/%.s,$(wildcard *.c))

all: $(TESTS)

$(WORKDIR)/%.s: %.c
	$(NOT) $(CC65) -o $@ $<

clean:
	@$(call DEL,$(TESTS))
