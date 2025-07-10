# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

# top-level Makefile for the regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  RMDIR = $(RM) -r $1
endif

ifeq ($(SILENT),s)
    QUIET := 1
endif

ifdef QUIET
    PQ = "QUIET=1"
    PD = --no-print-directory
endif

WORKDIR = ../testwrk/asm

.PHONY: all continue mostlyclean clean

all: mostlyclean continue

continue: mostlyclean
	@$(MAKE) $(PD) -C cpudetect all $(PQ)
	@$(MAKE) $(PD) -C opcodes all $(PQ)
	@$(MAKE) $(PD) -C listing all $(PQ)
	@$(MAKE) $(PD) -C val all $(PQ)
	@$(MAKE) $(PD) -C err all $(PQ)
	@$(MAKE) $(PD) -C misc all $(PQ)

mostlyclean:
	@$(MAKE) $(PD) -C cpudetect clean $(PQ)
	@$(MAKE) $(PD) -C opcodes clean $(PQ)
	@$(MAKE) $(PD) -C listing clean $(PQ)
	@$(MAKE) $(PD) -C val clean $(PQ)
	@$(MAKE) $(PD) -C err clean $(PQ)
	@$(MAKE) $(PD) -C misc clean $(PQ)

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
