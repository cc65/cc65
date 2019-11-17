;
; 2019-11-06, Greg King
;
; unsigned char kbhit (void);
; /* Returns non-zero (true) if a typed character is waiting. */
;

        .export         _kbhit

        .include        "cx16.inc"


.proc   _kbhit
        lda     KEY_COUNT       ; Get number of characters
        tax                     ; High byte of return (only its zero/nonzero ...
        rts			; ... state matters)
.endproc
