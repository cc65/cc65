;
; clock.s
;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; 2003-05-02, Greg King <gngking@erols.com>
;
; #include <time.h>
;
; clock_t clock (void);
;

        .include "nes.inc"

        .export         _clock
        .importzp       sreg


.proc   _clock

        ldy     #0              ; High word is always zero
        sty     sreg+1
        sty     sreg
L1:     ldx     tickcount+1
        lda     tickcount
        cpx     tickcount+1     ; Did tickcount change?
        bne     L1              ; Yes, re-read it
        rts

.endproc

