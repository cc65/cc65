all mostlyclean clean:
	@$(MAKE) -C src    --no-print-directory $@
	@$(MAKE) -C libsrc --no-print-directory $@

avail unavail bin:
	@$(MAKE) -C src    --no-print-directory $@

lib:
	@$(MAKE) -C libsrc --no-print-directory $@

%65:
	@$(MAKE) -C src    --no-print-directory $@

%:
	@$(MAKE) -C libsrc --no-print-directory $@

.PHONY: all mostlyclean clean avail unavail bin lib

.SUFFIXES:
