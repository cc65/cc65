;
; Ullrich von Bassewitz, 31.05.1998
;
; char* itoa (int value, char* s, int radix);
; char* utoa (unsigned value, char* s, int radix);
;

        .export         _itoa, _utoa
        .import         addysp1
        .import         __hextab
        .importzp       sp, sreg, ptr2, ptr3, tmp1

.rodata
specval:
        .byte   '-', '3', '2', '7', '6', '8', 0
.code

;
; Common subroutine to pop the parameters and put them into core
;

dopop:  sta     tmp1            ; will loose high byte
        ldy     #0
        lda     (sp),y
        sta     ptr2
        sta     ptr3
        iny
        lda     (sp),y
        sta     ptr2+1
        sta     ptr3+1
        iny
        lda     (sp),y
        sta     sreg
        iny
        lda     (sp),y
        sta     sreg+1
        jmp     addysp1         ; Bump stack pointer

;
; itoa
;

_itoa:  jsr     dopop           ; pop the arguments

; We must handle $8000 in a special way, since it is the only negative
; number that has no positive 16-bit counterpart

        ldy     tmp1            ; get radix
        cpy     #10
        bne     utoa
        cmp     #$00
        bne     L2
        cpx     #$80
        bne     L2

        ldy     #6
L1:     lda     specval,y       ; copy -32768
        sta     (ptr2),y
        dey
        bpl     L1
        jmp     L10

; Check if the value is negative. If so, write a - sign and negate the
; number.

L2:     lda     sreg+1          ; get high byte
        bpl     utoa
        lda     #'-'
        ldy     #0
        sta     (ptr2),y        ; store sign
        inc     ptr2
        bne     L3
        inc     ptr2+1

L3:     lda     sreg
        eor     #$FF
        clc
        adc     #$01
        sta     sreg
        lda     sreg+1
        eor     #$FF
        adc     #$00
        sta     sreg+1
        jmp     utoa

;
; utoa
;

_utoa:  jsr     dopop           ; pop the arguments

; Convert to string by dividing and push the result onto the stack

utoa:   lda     #$00
        pha                     ; sentinel

; Divide sreg/tmp1 -> sreg, remainder in a

L5:     ldy     #16             ; 16 bit
        lda     #0              ; remainder
L6:     asl     sreg
        rol     sreg+1
        rol     a
        cmp     tmp1
        bcc     L7
        sbc     tmp1
        inc     sreg
L7:     dey
        bne     L6

        tay                     ; get remainder into y
        lda     __hextab,y      ; get hex character
        pha                     ; save char value on stack

        lda     sreg
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














