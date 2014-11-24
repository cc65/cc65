
# toplevel makefile for the regression tests

MAKE := make --no-print-dir

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
RM := del /f
EXE := .exe
else
RM := rm -f
EXE :=
endif

.PHONY: dotests clean

all: dotests

bdiff:
	@$(CC) -o bdiff$(EXE) bdiff.c

dotests: bdiff
	@$(MAKE) -C val clean all
	@$(MAKE) -C ref clean all
	@$(MAKE) -C err clean all
	@$(MAKE) -C misc clean all

clean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean
	@$(RM) bdiff$(EXE)
