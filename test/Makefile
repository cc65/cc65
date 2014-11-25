
# toplevel makefile for the regression tests

MAKE := make --no-print-dir

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  RM := del /f
  EXE := .exe
  MKDIR = mkdir
else
  RM := rm -f
  EXE :=
  MKDIR = mkdir -p
endif

WORKDIR := ../testwrk

.PHONY: dotests clean

all: dotests

$(WORKDIR):
	@$(MKDIR) $(WORKDIR)

$(WORKDIR)/bdiff$(EXE): $(WORKDIR)
	@$(CC) -o $(WORKDIR)/bdiff$(EXE) bdiff.c

dotests: $(WORKDIR)/bdiff$(EXE)
	@$(MAKE) -C val clean all
	@$(MAKE) -C ref clean all
	@$(MAKE) -C err clean all
	@$(MAKE) -C misc clean all

clean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean
	@$(RM) $(WORKDIR)/bdiff$(EXE)
