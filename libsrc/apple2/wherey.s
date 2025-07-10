;
; Kevin Ruland
;
; unsigned char wherey (void);
;

        .export         _wherey

        .include        "apple2.inc"

_wherey:
        lda     CV
        sec
        sbc     WNDTOP
        ldx     #>$0000
        rts
