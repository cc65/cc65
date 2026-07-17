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
    QUIET = 1
endif

ifdef QUIET
    PQ = "QUIET=1"
    PD = --no-print-directory
endif

WORKDIR = ../testwrk

.PHONY: test continue mostlyclean clean success_message

test:
	@$(MAKE) mostlyclean
	@$(MAKE) continue

continue:
	@$(MAKE) $(PD) -C asm all $(PQ)
	@$(MAKE) $(PD) -C dasm all $(PQ)
	@$(MAKE) $(PD) -C val all $(PQ)
	@$(MAKE) $(PD) -C ref all $(PQ)
	@$(MAKE) $(PD) -C err all $(PQ)
	@$(MAKE) $(PD) -C standard all $(PQ)
	@$(MAKE) $(PD) -C standard_err all $(PQ)
	@$(MAKE) $(PD) -C misc all $(PQ)
	@$(MAKE) $(PD) -C todo all $(PQ)
	@$(MAKE) $(PD) -C dbginfo all $(PQ)
	@$(MAKE) success_message

success_message:
	$(info ###################################)
	$(info ### validation suite successful ###)
	$(info ###################################)

mostlyclean:
	@$(MAKE) $(PD) -C asm clean $(PQ)
	@$(MAKE) $(PD) -C dasm clean $(PQ)
	@$(MAKE) $(PD) -C val clean $(PQ)
	@$(MAKE) $(PD) -C ref clean $(PQ)
	@$(MAKE) $(PD) -C err clean $(PQ)
	@$(MAKE) $(PD) -C standard clean $(PQ)
	@$(MAKE) $(PD) -C standard_err clean $(PQ)
	@$(MAKE) $(PD) -C misc clean $(PQ)
	@$(MAKE) $(PD) -C todo clean $(PQ)
	@$(MAKE) $(PD) -C dbginfo clean $(PQ)

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
