;
; clock_t clock (void);
;

        .export         _clock
        .constructor    initclock

        .forceimport    ticktock        ; make sure that tickcount changes
        .importzp       sreg

        .include        "extzp.inc"

.proc   _clock
        lda     tickcount+3
        sta     sreg+1
        lda     tickcount+2
        sta     sreg
        ldx     tickcount+1
        lda     tickcount
        rts
.endproc


; Make the process clock start at zero.

        .segment        "ONCE"
initclock:
        lda     #0
        ldx     #3
@lp:    sta     tickcount,x
        dex
        bpl     @lp
        rts
