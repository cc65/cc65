;
; 2021-04-01, Greg King
;
; void waitvsync (void);
; /* Wait for the start of the next video field. */
;
; VERA's vertical sync causes IRQs which increment the jiffy timer.
;

        .export         _waitvsync

        .include        "cx16.inc"

_waitvsync:
        ldx     RAM_BANK        ; (TIMER is in RAM bank 0)
        stz     RAM_BANK
        lda     TIMER + 2
:       cmp     TIMER + 2
        beq     :-              ; Wait for next jiffy
        stx     RAM_BANK
        rts
