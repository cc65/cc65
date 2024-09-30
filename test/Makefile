# top-level Makefile for the regression tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  RMDIR = $(RM) -r $1
endif

WORKDIR = ../testwrk

.PHONY: test continue mostlyclean clean

test:
	@$(MAKE) mostlyclean
	@$(MAKE) continue

continue:
	@$(MAKE) -C asm all
	@$(MAKE) -C dasm all
	@$(MAKE) -C val all
	@$(MAKE) -C ref all
	@$(MAKE) -C err all
	@$(MAKE) -C standard all
	@$(MAKE) -C misc all
	@$(MAKE) -C todo all

mostlyclean:
	@$(MAKE) -C asm clean
	@$(MAKE) -C dasm clean
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C standard clean
	@$(MAKE) -C misc clean
	@$(MAKE) -C todo clean

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
