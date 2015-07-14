;
; clock_t clock (void);
;

        .include "pce.inc"

        .export         _clock
        .importzp       sreg
;;        .importzp       tickcount

.proc   _clock

        lda tickcount+3
        sta sreg+1
        lda tickcount+2
        sta sreg
        ldx tickcount+1
        lda tickcount
        rts

.endproc
