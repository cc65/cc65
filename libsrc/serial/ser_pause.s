;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_pause (void);
; /* Assert flow control and disable interrupts. */



        .include        "ser-kernel.inc"

        _ser_pause      = ser_pause

