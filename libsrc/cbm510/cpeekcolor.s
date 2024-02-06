;
; 2016-02-28, Groepaz
; 2017-06-19, Greg King
;
; unsigned char cpeekcolor (void);
;

        .export         _cpeekcolor

        .import         CURS_X: zp, CRAM_PTR: zp

        .include        "cbm510.inc"


_cpeekcolor:
        ldx     IndReg
        lda     #$0F
        sta     IndReg
        ldy     CURS_X
        lda     (CRAM_PTR),y    ; get color
        stx     IndReg
        and     #$0F
        ldx     #>$0000
        rts
