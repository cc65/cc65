
# toplevel makefile for the regression tests

MAKE := make --no-print-dir

.PHONY: all clean

all:
	@$(MAKE) -C val all
	@$(MAKE) -C ref all
	@$(MAKE) -C err all

clean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
