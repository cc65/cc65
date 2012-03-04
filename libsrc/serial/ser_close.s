;
; Ullrich von Bassewitz, 2003-08-22
;
; unsigned char ser_close (void);
; /* "Close" the port. Clear buffers and and disable interrupts. */


        .include        "ser-kernel.inc"

        _ser_close      = ser_close
