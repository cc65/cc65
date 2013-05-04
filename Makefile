all mostlyclean clean:
	@$(MAKE) -C src    --no-print-directory $@
	@$(MAKE) -C libsrc --no-print-directory $@

install uninstall:
	@$(MAKE) -C src    --no-print-directory $@

%65:
	@$(MAKE) -C src    --no-print-directory $@

%:
	@$(MAKE) -C libsrc --no-print-directory $@

.PHONY: all mostlyclean clean install uninstall

.SUFFIXES:
