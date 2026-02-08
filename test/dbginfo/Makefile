# Makefile for debug info tests

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  S = $(subst /,\,/)
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = -rmdir /s /q $(subst /,\,$1)
else
  S = /
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

CL65 := $(if $(wildcard ../../bin/cl65*),..$S..$Sbin$Scl65,cl65)

WORKDIR = ../../testwrk/dbginfo

C_SOURCES = val_types.c val_func.c
ASM_SOURCES = val_asm.s

TESTS  = $(C_SOURCES:%.c=$(WORKDIR)/%.dbg)
TESTS += $(ASM_SOURCES:%.s=$(WORKDIR)/%.dbg)

.PHONY: all clean compare update

all: $(TESTS) compare

$(WORKDIR):
	$(call MKDIR,$(WORKDIR))

# Compile C files
$(WORKDIR)/%.dbg: %.c | $(WORKDIR)
	$(CL65) -g -t sim6502 -o $(@:.dbg=.prg) -Wl --dbgfile,$@ $<

# Assemble ASM files
$(WORKDIR)/%.dbg: %.s | $(WORKDIR)
	$(CL65) -g -C test.cfg -o $(@:.dbg=.prg) -Wl --dbgfile,$@ $<

# Verify against golden files
compare: $(TESTS)
	@for dbg in $(TESTS); do \
		ref=$$(basename $$dbg .dbg).ref; \
		if [ -f $$ref ]; then \
			if ! diff -u $$dbg $$ref; then \
				echo "Test failed: $$dbg does not match $$ref"; \
				exit 1; \
			fi \
		else \
			echo "Warning: No golden file for $$dbg (run 'make update')"; \
		fi \
	done
	@echo "All dbginfo tests passed."

# Update golden files
update: $(TESTS)
	@for dbg in $(TESTS); do \
		ref=$$(basename $$dbg .dbg).ref; \
		echo "Updating $$ref"; \
		cp $$dbg $$ref; \
	done

clean:
	@$(call RMDIR,$(WORKDIR))
