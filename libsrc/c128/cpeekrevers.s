;
; 2016-02-28, Groepaz
; 2017-06-26, Greg King
;
; unsigned char cpeekrevers (void);
;

        .export         _cpeekrevers

        .include        "zeropage.inc"
        .include        "c128.inc"


_cpeekrevers:
        lda     MODE
        bmi     @c80

        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; get char

@return:
        and     #$80            ; get reverse flag
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts

@c80:
        lda     SCREEN_PTR
        ldy     SCREEN_PTR+1
        clc
        adc     CURS_X
        bcc     @s
        iny

        ; get byte from VDC mem
@s:     ldx     #VDC_DATA_LO
        stx     VDC_INDEX
@L0:    bit     VDC_INDEX
        bpl     @L0
        sta     VDC_DATA
        dex
        stx     VDC_INDEX
        sty     VDC_DATA

        ldx     #VDC_RAM_RW
        stx     VDC_INDEX
@L1:    bit     VDC_INDEX
        bpl     @L1             ; wait for blanking
        lda     VDC_DATA
        jmp     @return
