;
; Kevin Ruland
; Oleg A. Odintsov, Moscow, 2024
;
; unsigned char wherey (void);
;

        .export         _wherey

        .include        "agat.inc"

_wherey:
        lda     CV
        sec
        sbc     WNDTOP
        ldx     #$00
        rts
