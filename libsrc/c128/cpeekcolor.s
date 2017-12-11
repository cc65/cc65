;
; 2016-02-28, Groepaz
; 2017-06-26, Greg King
;
; unsigned char cpeekcolor (void);
;

        .export         _cpeekcolor

        .include        "c128.inc"


_cpeekcolor:
        bit     MODE
        bmi     @c80

        ldy     CURS_X
        lda     (CRAM_PTR),y    ; get color
        and     #$0F
        ldx     #>$0000
        rts

@c80:   lda     CRAM_PTR
        ldy     CRAM_PTR+1
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
        and     #$0F

; translate VDC->VIC colour

vdctovic:
        ldy     #$0F + 1
@L2:    dey
        cmp     $CE5C,y
        bne     @L2
        tya
        ldx     #>$0000
        rts
