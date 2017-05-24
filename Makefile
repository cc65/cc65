.PHONY: all mostlyclean clean install zip avail unavail bin lib doc html info samples

.SUFFIXES:

all mostlyclean clean install zip:
	@$(MAKE) -C src     --no-print-directory $@
	@$(MAKE) -C libsrc  --no-print-directory $@
	@$(MAKE) -C doc     --no-print-directory $@
	@$(MAKE) -C samples --no-print-directory $@

avail unavail bin:
	@$(MAKE) -C src     --no-print-directory $@

lib:
	@$(MAKE) -C libsrc  --no-print-directory $@

doc html info:
	@$(MAKE) -C doc     --no-print-directory $@

samples:
	@$(MAKE) -C samples --no-print-directory $@

%65:
	@$(MAKE) -C src     --no-print-directory $@

%:
	@$(MAKE) -C libsrc  --no-print-directory $@
