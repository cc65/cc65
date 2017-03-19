# Makefile for the tests that MUST NOT compile

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NOT = - # Hack
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  NOT = !
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

CC65 := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)

WORKDIR = ../../testwrk/err

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS = $(patsubst %.c,$(WORKDIR)/%.s,$(SOURCES))

all: $(TESTS)

$(WORKDIR)/%.s: %.c
	$(NOT) $(CC65) -o $@ $<

clean:
	@$(call RMDIR,$(WORKDIR))
