
CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
  $(info CC: $(CC))
endif

CFLAGS += -O3 -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS)

.PHONY: mostlyclean clean

gamate: gamate-fixcart

gamate-fixcart: gamate-fixcart.c
	$(CC) $(CFLAGS) -o gamate-fixcart gamate-fixcart.c

mostlyclean clean:
	$(RM) gamate-fixcart
	
install zip:
