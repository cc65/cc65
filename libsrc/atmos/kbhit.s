;
; Ullrich von Bassewitz, 2003-04-13
;
; int kbhit (void);
;

        .export         _kbhit

        .include        "atmos.inc"

_kbhit:
        ldx     #$00            ; Load high byte
        lda     KEYBUF          ; Flag for new char in bit 7
        asl     a               ; Shift bit 7 into carry
        txa                     ; A = 0
        rol     a               ; Move old bit 7 into bit 0
        rts

