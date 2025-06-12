
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

atari: ataricvt

ataricvt: ataricvt.c
	$(CC) $(CFLAGS) -o ataricvt ataricvt.c

mostlyclean clean:
	$(DEL) ataricvt
	
install zip:
