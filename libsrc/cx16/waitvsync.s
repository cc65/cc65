;
; 2019-09-26, Greg King
;
; void waitvsync (void);
;
; VERA's vertical sync. causes IRQs which increment the jiffy clock.
;

        .export         _waitvsync

        .include        "cx16.inc"

_waitvsync:
        lda     TIME + 2
:       cmp     TIME + 2
        beq     :-              ; Wait for next jiffy
        rts
