
CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
  $(info CC: $(CC))
endif

CFLAGS += -O3 -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS)

.PHONY: mostlyclean clean

zlib: warning
#zlib: deflater

warning:
	@echo "deflater needs zlib installed, use 'make deflater' to build"

deflater: deflater.c
	$(CC) $(CFLAGS) -o deflater deflater.c -lz

mostlyclean clean:
	$(RM) deflater
	
install zip:

