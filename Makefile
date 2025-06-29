# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

.PHONY: all mostlyclean clean install zip avail unavail bin lib doc html info samples test util checkstyle check checkprefix

.SUFFIXES:

all:
	@$(MAKE) -C src         --no-print-directory $@
	@$(MAKE) -C libsrc      --no-print-directory $@
	@$(MAKE) -C doc         --no-print-directory $@
	@$(MAKE) -C util        --no-print-directory $@
	@$(MAKE) -C samples     --no-print-directory $@
	@$(MAKE) checkprefix    --no-print-directory

zip:
	@$(MAKE) -C src         --no-print-directory $@
	@$(MAKE) -C libsrc      --no-print-directory $@
	@$(MAKE) -C doc         --no-print-directory $@
	@$(MAKE) -C util        --no-print-directory $@
	@$(MAKE) -C samples     --no-print-directory $@

install:
ifndef PREFIX
	$(error Error: PREFIX must be set for install to work)
endif
	@$(MAKE) -C src        --no-print-directory $@
	@$(MAKE) -C libsrc     --no-print-directory $@
	@$(MAKE) -C doc        --no-print-directory $@
	@$(MAKE) -C util       --no-print-directory $@
	@$(MAKE) -C samples    --no-print-directory $@

mostlyclean clean:
	@$(MAKE) -C src        --no-print-directory $@
	@$(MAKE) -C libsrc     --no-print-directory $@
	@$(MAKE) -C doc        --no-print-directory $@
	@$(MAKE) -C util       --no-print-directory $@
	@$(MAKE) -C samples    --no-print-directory $@
	@$(MAKE) -C test       --no-print-directory $@
	@$(MAKE) -C targettest --no-print-directory $@

avail unavail:
# FIXME: actually not true, PREFIX is ignored?
#ifndef PREFIX
#    $(error Error: PREFIX must be set for avail/unavail to work)
#endif
	@$(MAKE) -C src        --no-print-directory $@

bin:
	@$(MAKE) -C src         --no-print-directory $@
	@$(MAKE) checkprefix    --no-print-directory

lib libtest:
	@$(MAKE) -C libsrc     --no-print-directory $@

doc html info:
	@$(MAKE) -C doc        --no-print-directory $@

samples:
	@$(MAKE) -C samples    --no-print-directory $@

util:
	@$(MAKE) -C util       --no-print-directory $@

%65:
	@$(MAKE) -C src        --no-print-directory $@

%:
	@$(MAKE) -C libsrc     --no-print-directory $@

checkprefix:
ifndef PREFIX
	$(warning Warning: PREFIX is empty - make install will not work)
endif

# check the code style
checkstyle:
	@$(MAKE) -C .github/checks --no-print-directory $@

# check bsearch tables
sorted:
	@$(MAKE) -C .github/checks --no-print-directory $@

# check that no modules use "sp", requires the binaries to be built first
checksp:
	@$(MAKE) -C .github/checks --no-print-directory $@

# runs regression tests, requires libtest target libraries
test:
	@$(MAKE) -C test                 --no-print-directory $@

# GNU "check" target, which runs all tests
check:
	@$(MAKE) -C .github/checks checkstyle --no-print-directory
	@$(MAKE) -C .github/checks sorted     --no-print-directory
	@$(MAKE) -C src test                  --no-print-directory
	@$(MAKE) test
	@$(MAKE) -C targettest platforms      --no-print-directory
	@$(MAKE) -C samples platforms         --no-print-directory
