;
; Ullrich von Bassewitz, 2003-04-18
;
; unsigned char __fastcall__ ser_unpause (void);
; /* Re-enable interrupts and release flow control */


        .include        "ser-kernel.inc"

        _ser_unpause    = ser_unpause

