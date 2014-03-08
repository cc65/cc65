.PHONY: all mostlyclean clean install zip avail unavail bin lib doc

.SUFFIXES:

all mostlyclean clean install zip:
	@$(MAKE) -C src    --no-print-directory $@
	@$(MAKE) -C libsrc --no-print-directory $@
	@$(MAKE) -C doc    --no-print-directory $@

avail unavail bin:
	@$(MAKE) -C src    --no-print-directory $@

lib:
	@$(MAKE) -C libsrc --no-print-directory $@

doc:
	@$(MAKE) -C doc    --no-print-directory $@

%65:
	@$(MAKE) -C src    --no-print-directory $@

%:
	@$(MAKE) -C libsrc --no-print-directory $@
