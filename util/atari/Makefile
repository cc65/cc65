
CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
  $(info CC: $(CC))
endif

CFLAGS += -O3 -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS)

.PHONY: mostlyclean clean

atari: ataricvt

ataricvt: ataricvt.c
	$(CC) $(CFLAGS) -o ataricvt ataricvt.c

mostlyclean clean:
	$(RM) ataricvt
	
install zip:
