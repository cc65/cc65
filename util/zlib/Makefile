# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
endif

CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
  $(info CC: $(CC))
endif

ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  DEL = -del /f
else
  DEL = $(RM)
endif

CFLAGS += -O3 -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS)

.PHONY: mostlyclean clean

zlib:
#zlib: warning
#zlib: deflater

warning:
	@echo "util/zlib/deflater is no longer built by default"
	@echo "use 'make deflater' to build if you need it"
	@echo "note that you need zlib installed first"

deflater: deflater.c
	$(if $(QUIET),echo HOST:$@)
	$(CC) $(CFLAGS) -o deflater deflater.c -lz

mostlyclean clean:
	$(DEL) deflater

install zip:
