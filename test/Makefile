
# toplevel makefile for the regression tests

MAKE := make --no-print-dir

.PHONY: all clean

all:
	@$(MAKE) -C val clean all
	@$(MAKE) -C ref clean all
	@$(MAKE) -C err clean all
	@$(MAKE) -C misc clean all

clean:
	@$(MAKE) -C val clean
	@$(MAKE) -C ref clean
	@$(MAKE) -C err clean
	@$(MAKE) -C misc clean

