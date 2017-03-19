# top-level Makefile for the regression tests

# You can comment this special target when you debug the regression tests.
# Then, make will give you more progress reports.
.SILENT:

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  RMDIR = $(RM) -r $1
endif

WORKDIR = ../testwrk

.PHONY: all dotests continue mostlyclean clean

all: dotests

.NOTPARALLEL:

dotests: mostlyclean continue

continue:
	@$(MAKE) -C assembler all
	@$(MAKE) -C disassembler all
	@$(MAKE) -C val all
	@$(MAKE) -C ref all
	@$(MAKE) -C err all
	@$(MAKE) -C misc all

mostlyclean:
	@$(MAKE) -C assembler clean
	@$(MAKE) -C disassembler clean
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
