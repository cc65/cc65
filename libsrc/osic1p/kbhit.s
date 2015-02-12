;
; int kbhit (void);
;
; Currently a dummy function that returns always true

        .export         _kbhit

_kbhit:
        lda     #$01            ; load low byte with true value
        ldx     #$00            ; load high byte
        rts
