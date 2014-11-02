;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_put (char b);
; /* Send a character via the serial port. There is a transmit buffer, but
; ** transmitting is not done via interrupt. The function returns
; ** SER_ERR_OVERFLOW if there is no space left in the transmit buffer.
; */


        .include        "ser-kernel.inc"

        _ser_put        = ser_put

