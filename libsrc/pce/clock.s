;
; clock_t clock (void);
;

        .include "pce.inc"

        .export         _clock
        .importzp       sreg

.proc   _clock

        ldy #0                  ; Byte 3 is always zero
        sty sreg+1
        sty sreg

        ldx _tickcount+1
        lda _tickcount
        rts

.endproc
