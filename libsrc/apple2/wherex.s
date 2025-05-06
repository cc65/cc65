;
; Kevin Ruland
;
; unsigned char wherex (void);
;

        .export         _wherex

        .include        "apple2.inc"

_wherex:
        lda     CH
        .ifdef  __APPLE2ENH__
        bit     RD80VID         ; In 80 column mode?
        bpl     :+
        lda     OURCH
:       .endif
        ldx     #>$0000
        rts
