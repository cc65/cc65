# ---- Display info during parsing phase ----
$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~)
$(info ~~~ Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(MAKECMDGOALS),)
  $(info ~~~ Invoked target: (default))
else
  $(info ~~~ Invoked target: $(MAKECMDGOALS))
endif
$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~)


.PHONY: atari gamate zlib

util: atari gamate zlib

all: util

atari:
	@$(MAKE) -C atari --no-print-directory $@
gamate:
	@$(MAKE) -C gamate --no-print-directory $@
zlib:
	@$(MAKE) -C zlib --no-print-directory $@

mostlyclean clean:
	@$(MAKE) -C atari --no-print-directory $@
	@$(MAKE) -C gamate --no-print-directory $@
	@$(MAKE) -C zlib --no-print-directory $@

install zip:


