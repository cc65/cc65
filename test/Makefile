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

WORKDIR = ../testwrk

.PHONY: test continue mostlyclean clean success_message

test:
	@$(MAKE) mostlyclean
	@$(MAKE) continue

continue:
	@$(MAKE) -C asm all QUIET=$(QUIET)
	@$(MAKE) -C dasm all QUIET=$(QUIET)
	@$(MAKE) -C val all QUIET=$(QUIET)
	@$(MAKE) -C ref all QUIET=$(QUIET)
	@$(MAKE) -C err all QUIET=$(QUIET)
	@$(MAKE) -C standard all QUIET=$(QUIET)
	@$(MAKE) -C standard_err all QUIET=$(QUIET)
	@$(MAKE) -C misc all QUIET=$(QUIET)
	@$(MAKE) -C todo all QUIET=$(QUIET)
	@$(MAKE) success_message

success_message:
	$(info ###################################)
	$(info ### validation suite successful ###)
	$(info ###################################)

mostlyclean:
	@$(MAKE) -C asm clean QUIET=$(QUIET)
	@$(MAKE) -C dasm clean QUIET=$(QUIET)
	@$(MAKE) -C val clean QUIET=$(QUIET)
	@$(MAKE) -C ref clean QUIET=$(QUIET)
	@$(MAKE) -C err clean QUIET=$(QUIET)
	@$(MAKE) -C standard clean QUIET=$(QUIET)
	@$(MAKE) -C standard_err clean QUIET=$(QUIET)
	@$(MAKE) -C misc clean QUIET=$(QUIET)
	@$(MAKE) -C todo clean QUIET=$(QUIET)

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
