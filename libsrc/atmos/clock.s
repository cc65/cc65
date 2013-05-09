;
; Ullrich von Bassewitz, 2003-04-13
;
; clock_t clock (void);
;

        .export         _clock
        .import         negax
        .importzp       sreg

        .include        "atmos.inc"


.proc   _clock

; Clear the timer high 16 bits

        ldy     #$00
        sty     sreg
        sty     sreg+1

; Read the timer

        sei                     ; Disable interrupts
        lda     TIMER3
        ldx     TIMER3+1
        cli                     ; Reenable interrupts


; Since the timer is counting downwards, return the negated value

        jmp     negax

.endproc


