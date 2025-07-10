# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
    PQ = "QUIET=1"
    PD = --no-print-directory
endif

.PHONY: atari gamate zlib

util: atari gamate zlib

all: util

atari:
	@$(MAKE) -C atari $(PD) $(PQ)
gamate:
	@$(MAKE) -C gamate $(PD) $(PQ)
zlib:
	@$(MAKE) -C zlib $(PD) $(PQ)

mostlyclean clean:
	@$(MAKE) -C atari $(PD) $@ $(PQ)
	@$(MAKE) -C gamate $(PD) $@ $(PQ)
	@$(MAKE) -C zlib $(PD) $@ $(PQ)

install zip:


