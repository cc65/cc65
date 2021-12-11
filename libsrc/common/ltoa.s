;
; Ullrich von Bassewitz, 11.06.1998
;
; char* ltoa (long value, char* s, int radix);
; char* ultoa (unsigned long value, char* s, int radix);
;

        .export         _ltoa, _ultoa
        .import         popax, popptr1, negeax
        .import         __hextab, __longminstr
        .importzp       sreg, ptr1, ptr2, ptr3, tmp1

        .macpack        cpu

.code

;
; Common subroutine to pop the parameters and put them into core
;

dopop:  sta     tmp1            ; will loose high byte
        jsr     popax           ; get s to ptr2
        sta     ptr2
        stx     ptr2+1
        sta     ptr3            ; save for return
        stx     ptr3+1
        jsr     popptr1         ; get low word of value to ptr1
        jsr     popax           ; get high word of value to sreg
        sta     sreg
        stx     sreg+1
        rts

;
; ltoa
;

_ltoa:  jsr     dopop           ; pop the arguments

; We must handle $80000000 in a special way, since it is the only negative
; number that has no positive 32-bit counterpart

        ldx     sreg+1          ; get high byte
        ldy     tmp1            ; get radix
        cpy     #10
        bne     ultoa
        lda     sreg
        ora     ptr1+1
        ora     ptr1
        bne     L2
        cpx     #$80
        bne     L2

        ldy     #11
L1:     lda     __longminstr,y  ; copy -2147483648
        sta     (ptr2),y
        dey
        bpl     L1
        jmp     L10
                  
; Check if the value is negative. If so, write a - sign and negate the
; number.

L2:     txa                     ; get high byte
        bpl     ultoa
        lda     #'-'

.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (ptr2)
.else        
        ldy     #0
        sta     (ptr2),y        ; store sign
.endif

        inc     ptr2
        bne     L3
        inc     ptr2+1

L3:     lda     ptr1            ; negate val
        ldx     ptr1+1

        jsr     negeax
        
        sta     ptr1
        stx     ptr1+1
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
L6:     asl     ptr1
        rol     ptr1+1
        rol     sreg
        rol     sreg+1
        rol     a
        cmp     tmp1
        bcc     L7
        sbc     tmp1
        inc     ptr1
L7:     dey
        bne     L6

        tay                     ; get remainder into y
        lda     __hextab,y      ; get hex character
        pha                     ; save char value on stack

        lda     ptr1
        ora     ptr1+1
        ora     sreg
        ora     sreg+1
        bne     L5

; Get the characters from the stack into the string

        ldy     #0
L9:     pla
        sta     (ptr2),y
        beq     L10             ; jump if sentinel
        iny
        bne     L9              ; jump always

; Done! Return the target string

L10:    lda     ptr3
        ldx     ptr3+1
        rts














