;
; 2017-12-27, Groepaz
;
; unsigned char cpeekcolor (void);
;

        .export         soft80mono_cpeekcolor

        .include        "c64.inc"
        .include        "soft80.inc"

        .segment "CODE"

soft80mono_cpeekcolor:
        lda     CHARCOLOR
        ldx     #0
        rts
