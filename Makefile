.PHONY: all mostlyclean clean install zip avail unavail bin lib doc html info samples test util checkstyle check

.SUFFIXES:

all install zip:
	@$(MAKE) -C src     --no-print-directory $@
	@$(MAKE) -C libsrc  --no-print-directory $@
	@$(MAKE) -C doc     --no-print-directory $@
	@$(MAKE) -C util    --no-print-directory $@
	@$(MAKE) -C samples --no-print-directory $@

mostlyclean clean:
	@$(MAKE) -C src        --no-print-directory $@
	@$(MAKE) -C libsrc     --no-print-directory $@
	@$(MAKE) -C doc        --no-print-directory $@
	@$(MAKE) -C util       --no-print-directory $@
	@$(MAKE) -C samples    --no-print-directory $@
	@$(MAKE) -C test       --no-print-directory $@
	@$(MAKE) -C targettest --no-print-directory $@

avail unavail bin:
	@$(MAKE) -C src     --no-print-directory $@

lib libtest:
	@$(MAKE) -C libsrc  --no-print-directory $@

doc html info:
	@$(MAKE) -C doc     --no-print-directory $@

samples:
	@$(MAKE) -C samples --no-print-directory $@

util:
	@$(MAKE) -C util    --no-print-directory $@

%65:
	@$(MAKE) -C src     --no-print-directory $@

%:
	@$(MAKE) -C libsrc  --no-print-directory $@

# check the code style
checkstyle:
	@$(MAKE) -C .github/checks       --no-print-directory $@

# runs regression tests, requires libtest target libraries
test:
	@$(MAKE) -C test                 --no-print-directory $@

# GNU "check" target, which runs all tests
check:
	@$(MAKE) -C .github/checks checkstyle --no-print-directory
	@$(MAKE) test
	@$(MAKE) -C targettest platforms      --no-print-directory
	@$(MAKE) -C samples platforms         --no-print-directory
