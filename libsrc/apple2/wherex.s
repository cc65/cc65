;
; Kevin Ruland
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "apple2.inc"

_wherex:
        lda     CH
        ldx     #$00
        rts
