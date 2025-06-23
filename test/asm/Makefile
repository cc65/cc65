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

WORKDIR = ../testwrk/asm

.PHONY: all continue mostlyclean clean

all: mostlyclean continue

continue: mostlyclean
	@$(MAKE) -C cpudetect all
	@$(MAKE) -C opcodes all
	@$(MAKE) -C listing all
	@$(MAKE) -C val all
	@$(MAKE) -C err all
	@$(MAKE) -C misc all

mostlyclean:
	@$(MAKE) -C cpudetect clean
	@$(MAKE) -C opcodes clean
	@$(MAKE) -C listing clean
	@$(MAKE) -C val clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
