;
; Piotr Fusik, 18.11.2001
;
; unsigned long __fastcall__ adler32 (unsigned long adler, unsigned char* buf,
;                                     unsigned len);
;

        .export _adler32

        .import         incsp2, incsp4, popax, popeax
        .importzp       sreg, ptr1, ptr2, tmp1

BASE    =       65521   ; largest prime smaller than 65536

_adler32:
; ptr2 = (len & 0xff) == 0 ? len : len + 0x100;
        tay
        beq     @L1
        inx
@L1:    sta     ptr2
        stx     ptr2+1
; ptr1 = buf
        jsr     popax
        sta     ptr1
        stx     ptr1+1
; if (buf == NULL) return 1L;
        ora     ptr1+1
        beq     @L0
; s1 = adler & 0xFFFF; s2 = adler >> 16;
        jsr     popeax
; if (len == 0) return adler;
        ldy     ptr2
        bne     @L2
        ldy     ptr2+1
        beq     @RET
@L2:    ldy     #0
; s1 += *ptr++; if (s1 >= BASE) s1 -= BASE;
@L3:    clc
        adc     (ptr1),y
        bcc     @L4
        inx
        beq     @L5     ; C flag is set
@L4:    cpx     #>BASE
        bcc     @L6
        cmp     #<BASE
        bcc     @L6
        inx             ; ldx #0
@L5:    sbc     #<BASE  ; C flag is set
        clc
@L6:    sta     tmp1
; s2 += s1; if (s2 >= BASE) s2 -= BASE;
        adc     sreg    ; C flag is clear
        sta     sreg
        txa
        adc     sreg+1
        sta     sreg+1
        bcs     @L7
        cmp     #>BASE
        bcc     @L8
        lda     sreg
        cmp     #<BASE
        bcc     @L8
@L7:    lda     sreg
        sbc     #<BASE  ; C flag is set
        sta     sreg
        lda     sreg+1
        sbc     #>BASE
        sta     sreg+1
@L8:    lda     tmp1
        iny
        bne     @L9
        inc     ptr1+1
@L9:    dec     ptr2
        bne     @L3
        dec     ptr2+1
        bne     @L3
; return (s2 << 16) | s1;
@RET:   rts

; return 1L
@L0:    sta     sreg
        sta     sreg+1
        lda     #1
; ignore adler
        jmp     incsp4
