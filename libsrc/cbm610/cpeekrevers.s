;
; 2016-02-28, Groepaz
; 2017-06-19, Greg King
;
; unsigned char cpeekrevers (void);
;

        .export         _cpeekrevers

        .import         plot
        .import         CURS_X: zp, CharPtr: zp

        .include        "cbm610.inc"


_cpeekrevers:
        ldx     IndReg
        ldy     #$0F
        sty     IndReg

        ldy     CURS_X
        lda     (CharPtr),y     ; get char from system bank
        stx     IndReg
        ldx     #>$0000
        and     #$80            ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
