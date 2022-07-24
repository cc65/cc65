;
; 2017-07-05, Greg King
;
; void cpeeks (char* s, unsigned length);
;

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2
        .importzp       CURS_X, CharPtr

        .include        "cbm610.inc"
        .macpack        generic


_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr3            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr3+1

        lda     CharPtr
        ldx     CharPtr+1
        sta     ptr2
        stx     ptr2+1
        ldy     CURS_X
        sty     tmp2

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1
        ldx     IndReg
        ldy     #<$0000
        sty     ptr1
        bze     L3              ; branch always

L4:     ldy     #$0F
        sty     IndReg
        ldy     tmp2
        lda     (ptr2),y        ; get char from system bank
        stx     IndReg
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

        lda     #$00            ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts
