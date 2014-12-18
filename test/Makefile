
# top-level makefile for the regression tests

# You can comment this special target when you debug the regression tests.
# Then, make will give you more progress reports.
.SILENT:

ifneq ($(shell echo),)
  CMD_EXE := 1
endif

ifdef CMD_EXE
  RM := del /f
  EXE := .exe
  MKDIR := mkdir
  RMDIR := rmdir
else
  RM := rm -f
  EXE :=
  MKDIR := mkdir -p
  RMDIR := rmdir
endif

WORKDIR := ../testwrk

.PHONY: all dotests continue mostly-clean clean

all: dotests

$(WORKDIR):
	$(MKDIR) $(WORKDIR)

$(WORKDIR)/bdiff$(EXE): bdiff.c | $(WORKDIR)
	$(CC) -O2 -o $@ $<

.NOTPARALLEL:

dotests: mostly-clean continue

continue: $(WORKDIR)/bdiff$(EXE)
	@$(MAKE) -C val all
	@$(MAKE) -C ref all
	@$(MAKE) -C err all
	@$(MAKE) -C misc all

mostly-clean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean

clean: mostly-clean
	$(RM) $(WORKDIR)/bdiff$(EXE)
	$(RMDIR) $(WORKDIR)
