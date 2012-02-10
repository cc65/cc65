#
# Makefile
#

# --------------------------------------------------------------------------
# Programs

# Subdirectories
SUBDIRS = apple2 geos-apple

# --------------------------------------------------------------------------
# List of executables. This list could be made target dependent by checking
# $(SYS).

.PHONY: all
all:
	@for DIR in $(SUBDIRS); do \
	    $(MAKE) -C $${DIR} $(MAKEOPTS); \
	done

.PHONY:	clean
clean:
	@for DIR in $(SUBDIRS); do \
	    $(MAKE) -C $${DIR} $(MAKEOPTS) clean; \
	done

.PHONY:	zap
zap:
	@for DIR in $(SUBDIRS); do \
	    $(MAKE) -C $${DIR} $(MAKEOPTS) zap; \
	done

