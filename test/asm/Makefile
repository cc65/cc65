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

ifneq ($(QUIET),1)
    QUIET := 0
endif

WORKDIR = ../testwrk/asm

.PHONY: all continue mostlyclean clean

all: mostlyclean continue

continue: mostlyclean
	@$(MAKE) --no-print-directory -C cpudetect all QUIET=$(QUIET)
	@$(MAKE) --no-print-directory -C opcodes all QUIET=$(QUIET)
	@$(MAKE) --no-print-directory -C listing all QUIET=$(QUIET)
	@$(MAKE) --no-print-directory -C val all QUIET=$(QUIET)
	@$(MAKE) --no-print-directory -C err all QUIET=$(QUIET)
	@$(MAKE) --no-print-directory -C misc all QUIET=$(QUIET)

mostlyclean:
	@$(MAKE) --no-print-directory -C cpudetect clean
	@$(MAKE) --no-print-directory -C opcodes clean
	@$(MAKE) --no-print-directory -C listing clean
	@$(MAKE) --no-print-directory -C val clean
	@$(MAKE) --no-print-directory -C err clean
	@$(MAKE) --no-print-directory -C misc clean

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
