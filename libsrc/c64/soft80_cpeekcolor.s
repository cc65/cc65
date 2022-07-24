;
; 2017-12-27, Groepaz
;
; unsigned char cpeekcolor (void);
;

        .export         soft80_cpeekcolor

        .include        "c64.inc"
        .include        "soft80.inc"

        .segment "CODE"

soft80_cpeekcolor:
        ldy     #0
        lda     (CRAM_PTR),y
        and     #$0f
        ldx     #0
        rts
