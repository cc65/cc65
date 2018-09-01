;
; clock_t clock (void);
;

        .constructor    initclock
        .export         _clock

        .forceimport    ticktock        ; make sure that tickcount changes
        .importzp       tickcount, sreg


; Make the process clock start at zero.

        .segment        "ONCE"
initclock:
        ldx     #4 - 1
@lp:    stz     tickcount,x
        dex
        bpl     @lp
        rts

; ------------------------------------------------------------------------
.code

; This function might be interrupted while it is reading the several bytes of
; the clock.  They are read again if that happens.  (We do not want to stop
; interrupts because that might cause glitches in interrupt-driven graphics
; and sound.)

.proc   _clock
        lda     tickcount
        ldy     tickcount+3
        sty     sreg+1
        ldy     tickcount+2
        sty     sreg
        ldx     tickcount+1
        cmp     tickcount
        bne     _clock                  ; clock changed; reread it
        rts
.endproc
