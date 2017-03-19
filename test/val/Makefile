# Makefile for the regression tests that return an error code on failure

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  DEL = del /f $(subst /,\,$1)
else
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  DEL = $(RM) $1
endif

CC65FLAGS = -t sim6502
SIM65FLAGS = -x 200000000

CL65 := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
SIM65 := $(if $(wildcard ../../bin/sim65*),../../bin/sim65,sim65)

WORKDIR = ../../testwrk/val

OPTIONS = g O Os Osi Osir Oi Oir Or

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).prg))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

# Some files have "K & R"-style syntax.  Therefore, some forward
# function-declarations don't match the later function definitions.
# Those programs fail when fastcall is used; but, the cdecl calling convention
# tolerates those conflicts.  Therefore, make their functions default to cdecl.
#
$(WORKDIR)/cq4.%.prg  \
$(WORKDIR)/cq71.%.prg \
$(WORKDIR)/cq81.%.prg \
$(WORKDIR)/cq84.%.prg: CC65FLAGS += -Wc --all-cdecl

define PRG_template

$(WORKDIR)/%.$1.prg: %.c | $(WORKDIR)
	$(CL65) $$(CC65FLAGS) -$1 -o $$@ $$<
	$(SIM65) $(SIM65FLAGS) $$@

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option))))

clean:
	@$(call RMDIR,$(WORKDIR))
	@$(call DEL,$(SOURCES:.c=.o))
