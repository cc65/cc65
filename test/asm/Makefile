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

SUBDIRS = cpudetect opcodes listing val err misc

.PHONY: all continue mostlyclean clean

all: mostlyclean continue

define CALL_template

continue::
	@$(MAKE) -C $1 all

mostlyclean::
	@$(MAKE) -C $1 clean

endef

$(foreach subdir,$(SUBDIRS),$(eval $(call CALL_template,$(subdir))))

clean: mostlyclean
	@$(call RMDIR,$(WORKDIR))
