;
; Kevin Ruland
;
; unsigned char wherex (void);
;

        .ifndef __APPLE2ENH__
        .import         machinetype
        .endif

        .export         _wherex

        .include        "apple2.inc"

_wherex:
        lda     CH
        .ifndef __APPLE2ENH__
        bit     machinetype
        bpl     :+
        .endif
        bit     RD80VID         ; In 80 column mode?
        bpl     :+
        lda     OURCH

:       ldx     #>$0000
        rts
