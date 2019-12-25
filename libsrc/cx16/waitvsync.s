;
; 2019-12-23, Greg King
;
; void waitvsync (void);
; /* Wait for the start of the next video field. */
;
; VERA's vertical sync causes IRQs which increment the jiffy timer.
;

        .export         _waitvsync

        .include        "cx16.inc"

_waitvsync:
        ldx     VIA1::PRA2      ; (TIMER is in RAM bank 0)
        stz     VIA1::PRA2
        lda     TIMER + 2
:       cmp     TIMER + 2
        beq     :-              ; Wait for next jiffy
        stx     VIA1::PRA2
        rts
