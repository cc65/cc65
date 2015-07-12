;
; clock_t clock (void);
;

        .include "pce.inc"

        .export         _clock
        .importzp       sreg

.proc   _clock

        lda _tickcount+3
        sta sreg+1
        lda _tickcount+2
        sta sreg
        ldx _tickcount+1
        lda _tickcount
        rts

.endproc
