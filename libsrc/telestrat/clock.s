;
; Jede, 2021-03-10
;
; clock_t clock (void);
;

        .export         _clock
        .importzp       sreg

        .include        "telestrat.inc"

.proc   _clock

; Clear the timer high 16 bits

        ldy     #$00
        sty     sreg
        sty     sreg+1

; Read the timer

        sei                     ; Disable interrupts
        lda     TIMEUD          ; TIMED contains 1/10 of a second from clock. Telestrat main cardridge simulate a clock from VIA6522 timer
        ldx     TIMEUD+1
        cli                     ; Reenable interrupts

        rts
.endproc
