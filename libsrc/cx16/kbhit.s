;
; 2019-09-20, Greg King
;
; unsigned char kbhit (void);
; /* Returns non-zero (true) if a typed character is waiting. */
;

        .export         _kbhit

        .include        "cx16.inc"


.proc   _kbhit
        ldx     #>$0000         ; High byte of return
        lda     KEY_COUNT       ; Get number of characters
        rts
.endproc
