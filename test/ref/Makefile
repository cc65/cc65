# Makefile for the regression tests that generate output which has to be
# compared with reference output

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  EXE = .exe
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
  COPY = copy $(subst /,\,$1) $(subst /,\,$2)
else
  S = /
  EXE =
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
  COPY = cp $1 $2
endif

ifdef QUIET
  .SILENT:
  NULLERR = 2>$(NULLDEV)
endif

SIM65FLAGS = -x 200000000

CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)
CA65 := $(if $(wildcard ../../bin/ca65*),..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ..$S..$Stestwrk$Sref

OPTIONS = g O Os Osi Osir Osr Oi Oir Or

ISEQUAL = ..$S..$Stestwrk$Sisequal$(EXE)

# NOTE: the current test bench may include K&R style C, C89 style C, C99 - and
#       even things from later standards. Technically C99 removed certain C89
#       constructs - However, so far GCC would still compile them and issue a
#       warning (instead of an error). Now, GCC 14 will be more strict about this,
#       and by default make those things an error instead. We use -std=gnu17 here
#       so we can still build the references with a modern compiler, and don't
#       have to deal with special-casing individual tests that use constructs
#       from those old standards. Should this become a problem in the future, we
#       will have to change that, and create said special cases here.
#       see discussion in https://github.com/cc65/cc65/issues/2277
CC = gcc
CFLAGS = -std=gnu17 -O2 -Wall -W -Wextra -funsigned-char -fwrapv -fno-strict-overflow -Wno-error=implicit-int -Wno-error=int-conversion

.PHONY: all clean

# list of sources that produces warnings that we want to check. a .cref file
# containing the exact output is required.
CUSTOMSOURCES = \
	custom-reference.c

# list of sources that produce a compiler error. a .cref files containing the
# exact error output is required
ERRORSOURCES = \
	custom-reference-error.c \
	inline-error.c \
	bug1889-missing-identifier.c \
	bug2312-preprocessor-error.c

SOURCES := $(filter-out $(CUSTOMSOURCES) $(ERRORSOURCES),$(wildcard *.c))

REFS = $(SOURCES:%.c=$(WORKDIR)/%.ref)
CUSTOMREFS = $(CUSTOMSOURCES:%.c=$(WORKDIR)/%.cref) $(ERRORSOURCES:%.c=$(WORKDIR)/%.cref)

TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

CUSTOMTESTS  = $(foreach option,$(OPTIONS),$(CUSTOMSOURCES:%.c=$(WORKDIR)/%.$(option).6502.custom.prg))
CUSTOMTESTS += $(foreach option,$(OPTIONS),$(CUSTOMSOURCES:%.c=$(WORKDIR)/%.$(option).65c02.custom.prg))

ERRORTESTS  = $(foreach option,$(OPTIONS),$(ERRORSOURCES:%.c=$(WORKDIR)/%.$(option).6502.error.prg))
ERRORTESTS += $(foreach option,$(OPTIONS),$(ERRORSOURCES:%.c=$(WORKDIR)/%.$(option).65c02.error.prg))

all: $(CUSTOMREFS) $(REFS) $(TESTS) $(CUSTOMTESTS) $(ERRORTESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(WORKDIR)/%.cref: %.cref | $(WORKDIR)
	$(if $(QUIET),echo ref/$*.cref)
	$(call COPY,$*.cref,$@)

$(WORKDIR)/%.ref: %.c | $(WORKDIR)
	$(if $(QUIET),echo ref/$*.host)
	$(CC) $(CFLAGS) -o $(WORKDIR)/$*.host $< $(NULLERR)
	$(WORKDIR)$S$*.host > $@

# "yaccdbg.c" includes "yacc.c".
# yaccdbg's built files must depend on both of them.
#
$(WORKDIR)/yaccdbg.ref:   yacc.c
$(WORKDIR)/yaccdbg.%.prg: yacc.c

define PRG_template

$(WORKDIR)/%.$1.$2.prg: %.c $(WORKDIR)/%.ref $(ISEQUAL)
	$(if $(QUIET),echo ref/$$*.$1.$2.prg)
	$(CC65) -t sim$2 $$(CC65FLAGS) -$1 -o $$(@:.prg=.s) $$< $(NULLERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$*.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/$$*.$1.$2.out $(WORKDIR)/$$*.ref

endef # PRG_template

# extra template for the case when compilation works, but we still want to
# compare the warning output with our custom reference
define PRG_custom_template

$(WORKDIR)/%.$1.$2.custom.prg: %.c $(WORKDIR)/%.ref %.c $(WORKDIR)/%.cref $(ISEQUAL)
	$(if $(QUIET),echo cref/$$*.$1.$2.custom.prg)
	-$(CC65) -t sim$2 $$(CC65FLAGS) -$1 -o $$(@:.custom.prg=.s) $$< 2> $(WORKDIR)/$$*.$1.$2.cout
	$(CA65) -t sim$2 -o $$(@:.custom.prg=.o) $$(@:.custom.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.custom.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$*.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/$$*.$1.$2.cout $(WORKDIR)/$$*.cref
	$(ISEQUAL) $(WORKDIR)/$$*.$1.$2.out $(WORKDIR)/$$*.ref

endef # PRG_error_template

# extra template for the case when compilation fails, but we still want to
# compare the error output with our custom reference
define PRG_error_template

$(WORKDIR)/%.$1.$2.error.prg: %.c $(WORKDIR)/%.cref $(ISEQUAL)
	$(if $(QUIET),echo cref/$$*.$1.$2.error.prg)
	-$(CC65) -t sim$2 $$(CC65FLAGS) -$1 -o $$(@:.error.prg=.s) $$< 2> $(WORKDIR)/$$*.$1.$2.cout
#	$(CA65) -t sim$2 -o $$(@:.error.prg=.o) $$(@:.error.prg=.s) $(NULLERR)
#	$(LD65) -t sim$2 -o $$@ $$(@:.error.prg=.o) sim$2.lib $(NULLERR)
#	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/$$*.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/$$*.$1.$2.cout $(WORKDIR)/$$*.cref

endef # PRG_error_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

$(foreach option,$(OPTIONS),$(eval $(call PRG_custom_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_custom_template,$(option),65c02)))

$(foreach option,$(OPTIONS),$(eval $(call PRG_error_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_error_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
