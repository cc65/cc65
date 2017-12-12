;
; 2017-07-05, Greg King
; 2017-12-12, Groepaz
;
; void cpeeks (char* s, unsigned length);
;

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

        .macpack        generic

; FIXME c128 needs special version that handles the 80-column VDC.

        .include        "c128.inc"

_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr3            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr3+1

        lda     MODE
        bmi     c80

        lda     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sta     ptr2
        stx     ptr2+1
        ldy     CURS_X
        sty     tmp2

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1
        ldx     #<$0000
        stx     ptr1
        bze     L3              ; branch always

L4:     ldy     tmp2
        lda     (ptr2),y        ; get char
        iny
        bnz     L2
        inc     ptr2+1
L2:     sty     tmp2
        and     #<~$80          ; remove reverse bit

; Convert the screen code into a PetSCII code.
; $00 - $1F: +$40
; $20 - $3F
; $40 - $5f: +$20
; $60 - $7F: +$40

        cmp     #$20
        blt     @sk1            ;(bcc)
        cmp     #$40
        blt     L5
        cmp     #$60
        blt     @sk2            ;(bcc)
        clc
@sk1:   adc     #$20
@sk2:   ;clc                    ; both above cmp and adc clear carry flag
        adc     #$20

L5:     ldy     tmp1
        sta     (ptr1),y
        iny
        bnz     L1
        inc     ptr1+1
L1:     sty     tmp1

L3:     inc     ptr3            ; count length
        bnz     L4
        inc     ptr3+1
        bnz     L4

        txa                     ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts

        ;-----------------------------------------------------------
c80:
        lda     SCREEN_PTR
        clc
        adc     CURS_X
        sta     ptr2
        lda     SCREEN_PTR+1
        adc     #0
        sta     ptr2+1

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1
        ldx     #<$0000
        stx     ptr1
        bze     L3a              ; branch always

L4a:
        lda     ptr2
        ldy     ptr2+1
        inc     ptr2
        bne     @s
        inc     ptr2+1
@s:
        ; get byte from VDC mem
        ldx     #VDC_DATA_LO
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

        and     #<~$80          ; remove reverse bit

; Convert the screen code into a PetSCII code.
; $00 - $1F: +$40
; $20 - $3F
; $40 - $5f: +$20
; $60 - $7F: +$40

        cmp     #$20
        blt     @sk1            ;(bcc)
        cmp     #$40
        blt     L5a
        cmp     #$60
        blt     @sk2            ;(bcc)
        clc
@sk1:   adc     #$20
@sk2:   ;clc                    ; both above cmp and adc clear carry flag
        adc     #$20

L5a:    ldy     tmp1
        sta     (ptr1),y
        iny
        bnz     L1a
        inc     ptr1+1
L1a:    sty     tmp1

L3a:    inc     ptr3            ; count length
        bnz     L4a
        inc     ptr3+1
        bnz     L4a

        lda     #0              ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts
