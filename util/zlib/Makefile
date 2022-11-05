
CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
  $(info CC: $(CC))
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
	$(CC) $(CFLAGS) -o deflater deflater.c -lz

mostlyclean clean:
	$(RM) deflater

install zip:
