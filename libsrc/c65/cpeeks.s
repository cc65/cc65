;
; 2017-07-05, Greg King
;
; void cpeeks (char* s, unsigned length);
;

        .include "c65.inc"

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

        .macpack        generic

_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr3            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr3+1

        lda     SCREEN_PTR
        sta     ptr2
        lda     SCREEN_PTR+1
        clc
        adc     #>$0800
        sta     ptr2+1

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
