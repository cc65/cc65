# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# Makefile for the remaining tests that need special care in one way or another

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  NOT = - # Hack
  EXE = .exe
  NULLDEV = nul:
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  S = /
  NOT = !
  EXE =
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
  .SILENT:
  NULLOUT = >$(NULLDEV)
  NULLERR = 2>$(NULLDEV)
ifndef CMD_EXE
  CATERR = 2> $(WORKDIR)/$$@.errlog || (cat $(WORKDIR)/$$@.errlog && false)
endif
endif

SIM65FLAGS = -x 200000000

CC65 := $(if $(wildcard ../../bin/cc65*),..$S..$Sbin$Scc65,cc65)
CA65 := $(if $(wildcard ../../bin/ca65*),..$S..$Sbin$Sca65,ca65)
LD65 := $(if $(wildcard ../../bin/ld65*),..$S..$Sbin$Sld65,ld65)
SIM65 := $(if $(wildcard ../../bin/sim65*),..$S..$Sbin$Ssim65,sim65)

WORKDIR = ..$S..$Stestwrk$Smisc

OPTIONS = g O Os Osi Osir Osr Oi Oir Or

ISEQUAL = ..$S..$Stestwrk$Sisequal$(EXE)

CC = gcc
CFLAGS = -O2

.PHONY: all clean

SOURCES := $(wildcard *.c)
TESTS  = $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).6502.prg))
TESTS += $(foreach option,$(OPTIONS),$(SOURCES:%.c=$(WORKDIR)/%.$(option).65c02.prg))

all: $(TESTS)

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(ISEQUAL): ../isequal.c | $(WORKDIR)
	$(CC) $(CFLAGS) -o $@ $<

define PRG_template

# should compile, but gives an error
$(WORKDIR)/bug1209-ind-goto-rev.$1.$2.prg: bug1209-ind-goto-rev.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(if $(QUIET),echo misc/bug1209-ind-goto-rev.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(NOT) $(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)

# should compile, but gives an error
$(WORKDIR)/bug1209-ind-goto-rev-2.$1.$2.prg: bug1209-ind-goto-rev-2.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(if $(QUIET),echo misc/bug1209-ind-goto-rev-2.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(NOT) $(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)

# should compile, but gives an error
$(WORKDIR)/bug1209-ind-goto-rev-3.$1.$2.prg: bug1209-ind-goto-rev-3.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(if $(QUIET),echo misc/bug1209-ind-goto-rev-3.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(NOT) $(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)

# should compile, but gives an error
$(WORKDIR)/pptest2.$1.$2.prg: pptest2.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(if $(QUIET),echo misc/pptest2.$1.$2.prg)
	$(NOT) $(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) $(CATERR)

# should compile, but compiler exits with internal error
$(WORKDIR)/bug1211-ice-move-refs-2.$1.$2.prg: bug1211-ice-move-refs-2.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(if $(QUIET),echo misc/bug1211-ice-move-refs-2.$1.$2.prg)
	$(NOT) $(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) $(CATERR)

# this one requires --std=c89, it fails with --std=c99
$(WORKDIR)/bug1265.$1.$2.prg: bug1265.c | $(WORKDIR)
	$(if $(QUIET),echo misc/bug1265.$1.$2.prg)
	$(CC65) --standard c89 -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT) $(NULLERR)

# should not compile, but gives different diagnostics in C99 mode than in others
$(WORKDIR)/bug2515.$1.$2.prg: bug2515.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/bug2515.$1.$2.prg)
	$(NOT) $(CC65) --standard c99 -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) 2>$(WORKDIR)/bug2515.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/bug2515.$1.$2.out bug2515.c99.ref
	$(NOT) $(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) 2>$(WORKDIR)/bug2515.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/bug2515.$1.$2.out bug2515.ref

# should not issue any warnings in C99 mode
$(WORKDIR)/bug2637.$1.$2.prg: bug2637.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/bug2637.$1.$2.prg)
	$(CC65) --standard c99 -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) 2>$(WORKDIR)/bug2637.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/bug2637.$1.$2.out bug2637.ref

# this one requires -Werror
$(WORKDIR)/bug1768.$1.$2.prg: bug1768.c | $(WORKDIR)
	$(if $(QUIET),echo misc/bug1768.$1.$2.prg)
	$(CC65) -Werror -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT) $(NULLERR)

# should compile, but then hangs in an endless loop
$(WORKDIR)/endless.$1.$2.prg: endless.c | $(WORKDIR)
	$(if $(QUIET),echo misc/endless.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(NOT) $(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT) $(NULLERR)

# these need reference data that can't be generated by a host-compiled program,
# in a useful way
$(WORKDIR)/bug2655.$1.$2.prg: bug2655.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/bug2655.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/bug2655.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/bug2655.$1.$2.out bug2655.ref

$(WORKDIR)/flow-do-01.$1.$2.prg: flow-do-01.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-do-01.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-do-01.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-do-01.$1.$2.out flow-do-01.ref

$(WORKDIR)/flow-if-01.$1.$2.prg: flow-if-01.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-if-01.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-if-01.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-if-01.$1.$2.out flow-if-01.ref

$(WORKDIR)/flow-if-02.$1.$2.prg: flow-if-02.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-if-02.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-if-02.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-if-02.$1.$2.out flow-if-02.ref

$(WORKDIR)/flow-switch-01.$1.$2.prg: flow-switch-01.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-switch-01.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-switch-01.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-switch-01.$1.$2.out flow-switch-01.ref

$(WORKDIR)/flow-while-01.$1.$2.prg: flow-while-01.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-while-01.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-while-01.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-while-01.$1.$2.out flow-while-01.ref

$(WORKDIR)/flow-while-02.$1.$2.prg: flow-while-02.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/flow-while-02.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/flow-while-02.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/flow-while-02.$1.$2.out flow-while-02.ref

$(WORKDIR)/limits.$1.$2.prg: limits.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/limits.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$(@:.prg=.s) $$< $(NULLOUT) $(CATERR)
	$(CA65) -t sim$2 -o $$(@:.prg=.o) $$(@:.prg=.s) $(NULLERR)
	$(LD65) -t sim$2 -o $$@ $$(@:.prg=.o) sim$2.lib $(NULLERR)
	$(SIM65) $(SIM65FLAGS) $$@ > $(WORKDIR)/limits.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/limits.$1.$2.out limits.ref

$(WORKDIR)/goto.$1.$2.prg: goto.c $(ISEQUAL) | $(WORKDIR)
	$(if $(QUIET),echo misc/goto.$1.$2.prg)
	$(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) 2>$(WORKDIR)/goto.$1.$2.out
	$(ISEQUAL) $(WORKDIR)/goto.$1.$2.out goto.ref

# this one requires failure with --std=c89, it fails with --std=cc65 due to
# stricter checks
$(WORKDIR)/bug2304-implicit-func.$1.$2.prg: bug2304-implicit-func.c | $(WORKDIR)
	$(if $(QUIET),echo misc/bug2304-implicit-func.$1.$2.prg)
	$(NOT) $(CC65) --standard c89 -t sim$2 -$1 -o $$@ $$< $(NULLOUT) $(CATERR)

# should not compile until 3-byte struct by value tests are re-enabled
$(WORKDIR)/struct-by-value.$1.$2.prg: struct-by-value.c | $(WORKDIR)
	$(if $(QUIET),echo misc/struct-by-value.$1.$2.prg)
	$(NOT) $(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) $(CATERR)

# the rest are tests that fail currently for one reason or another
$(WORKDIR)/sitest.$1.$2.prg: sitest.c | $(WORKDIR)
	@echo "FIXME: " $$@ "currently does not compile."
	$(NOT) $(CC65) -t sim$2 -$1 -o $$@ $$< $(NULLOUT) $(CATERR)
#	$(SIM65) $(SIM65FLAGS) $$@ $(NULLOUT)

endef # PRG_template

$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),6502)))
$(foreach option,$(OPTIONS),$(eval $(call PRG_template,$(option),65c02)))

clean:
	@$(call RMDIR,$(WORKDIR))
