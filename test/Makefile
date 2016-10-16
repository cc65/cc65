
# top-level makefile for the regression tests

# You can comment this special target when you debug the regression tests.
# Then, make will give you more progress reports.
.SILENT:

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  EXE := .exe
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  EXE :=
  MKDIR = mkdir $1
  RMDIR = $(RM) -r $1
endif

WORKDIR := ../testwrk

CC := gcc

.PHONY: all dotests continue mostlyclean clean

all: dotests

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

$(WORKDIR)/bdiff$(EXE): bdiff.c | $(WORKDIR)
	$(CC) -O2 -o $@ $<

.NOTPARALLEL:

dotests: mostlyclean continue

continue: $(WORKDIR)/bdiff$(EXE)
	@$(MAKE) -C assembler all
	@$(MAKE) -C disassembler all
	@$(MAKE) -C val all
	@$(MAKE) -C ref all
	@$(MAKE) -C err all
	@$(MAKE) -C misc all

mostlyclean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
