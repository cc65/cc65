;
; Ullrich von Bassewitz, 11.06.1998
;
; char* ltoa (long value, char* s, int radix);
; char* ultoa (unsigned long value, char* s, int radix);
;

        .export         _ltoa, _ultoa
        .import         popax
        .import         __hextab, __longminstr
        .importzp       sreg, ptr1, ptr2, ptr3, tmp1



.code

;
; Common subroutine to pop the parameters and put them into core
;

dopop:  sta     tmp1            ; will loose high byte
        jsr     popax           ; get s
        sta     ptr1
        stx     ptr1+1
        sta     sreg            ; save for return
        stx     sreg+1
        jsr     popax           ; get low word of value
        sta     ptr2
        stx     ptr2+1
        jsr     popax           ; get high word of value
        sta     ptr3
        stx     ptr3+1
        rts

;
; ltoa
;

_ltoa:  jsr     dopop           ; pop the arguments

; We must handle $80000000 in a special way, since it is the only negative
; number that has no positive 32-bit counterpart

        ldx     ptr3+1          ; get high byte
        ldy     tmp1            ; get radix
        cpy     #10
        bne     ultoa
        lda     ptr3
        ora     ptr2+1
        ora     ptr2
        bne     L2
        cpx     #$80
        bne     L2

        ldy     #11
L1:     lda     __longminstr,y  ; copy -2147483648
        sta     (ptr1),y
        dey
        bpl     L1
        jmp     L10
                  
; Check if the value is negative. If so, write a - sign and negate the
; number.

L2:     txa                     ; get high byte
        bpl     ultoa
        lda     #'-'
        ldy     #0
        sta     (ptr1),y        ; store sign
        inc     ptr1
        bne     L3
        inc     ptr1+1

L3:     lda     ptr2            ; negate val
        eor     #$FF
        clc
        adc     #$01
        sta     ptr2
        lda     ptr2+1
        eor     #$FF
        adc     #$00
        sta     ptr2+1
        lda     ptr3
        eor     #$FF
        adc     #$00
        sta     ptr3
        lda     ptr3+1
        eor     #$FF
        adc     #$00
        sta     ptr3+1
        jmp     ultoa

;
; utoa
;

_ultoa: jsr     dopop           ; pop the arguments

; Convert to string by dividing and push the result onto the stack

ultoa:  lda     #$00
        pha                     ; sentinel

; Divide val/tmp1 -> val, remainder in a

L5:     ldy     #32             ; 32 bit
        lda     #0              ; remainder
L6:     asl     ptr2
        rol     ptr2+1
        rol     ptr3
        rol     ptr3+1
        rol     a
        cmp     tmp1
        bcc     L7
        sbc     tmp1
        inc     ptr2
L7:     dey
        bne     L6

        tay                     ; get remainder into y
        lda     __hextab,y      ; get hex character
        pha                     ; save char value on stack

        lda     ptr2
        ora     ptr2+1
        ora     ptr3
        ora     ptr3+1
        bne     L5

; Get the characters from the stack into the string

        ldy     #0
L9:     pla
        sta     (ptr1),y
        beq     L10             ; jump if sentinel
        iny
        bne     L9              ; jump always

; Done! Return the target string

L10:    lda     sreg
        ldx     sreg+1
        rts














