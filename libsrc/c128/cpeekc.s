;
; 2016-02-28, Groepaz
; 2017-06-26, Greg King
;
; char cpeekc (void);
;

        .export         _cpeekc

        .import         plot, popa

        .include        "zeropage.inc"
        .include        "c128.inc"


_cpeekc:
        lda     MODE
        bmi     @c80

        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; get char

@return:
        and     #<~$80          ; remove reverse flag

; Convert the screen code into a PetSCII code.
; $00 - $1F: +$40
; $20 - $3F
; $40 - $5f: +$20
; $60 - $7F: +$40

        cmp     #$20
        bcs     @sk1            ;(bge)
        ora     #$40
        rts

@sk1:   cmp     #$40
        bcc     @end            ;(blt)
        cmp     #$60
        bcc     @sk2            ;(blt)
        ;sec
        adc     #$20 - $01
@sk2:   ;clc                    ; both above cmp and adc clear carry flag
        adc     #$20
@end:   rts

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
