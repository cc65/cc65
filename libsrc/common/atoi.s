;
; Ullrich von Bassewitz, 05.06.1998
;
; int atoi (const char* s);
; long atol (const char* s);
;

        .export         _atoi, _atol
        .import         negeax, __ctype
        .importzp       sreg, ptr1, ptr2, tmp1
        .import         ctypemaskdirect
        .include        "ctype.inc"
;
; Conversion routine (32 bit)
;

_atoi:
_atol:  sta     ptr1            ; store s
        stx     ptr1+1
        ldy     #0
        sty     ptr2
        sty     ptr2+1          ; initial value (32 bit)
        sty     sreg
        sty     sreg+1

; Skip whitespace

L1:     lda     (ptr1),y
        jsr     ctypemaskdirect ; get character classification
        and     #CT_SPACE_TAB   ; tab or space?
        beq     L2              ; jump if no
        iny
        bne     L1
        inc     ptr1+1
        bne     L1              ; branch always

; Check for a sign. Refetch character, X is cleared by preprocessor

L2:     lda     (ptr1),y        ; get char
                                ; x=0 -> flag: positive
        cmp     #'+'            ; ### portable?
        beq     L3
        cmp     #'-'            ; ### portable?
        bne     L5
        dex                     ; flag: negative
L3:     iny
        bne     L5
        inc     ptr1+1

; Store the sign flag and setup for conversion

L5:     stx     tmp1            ; remember sign flag

L6:     lda     (ptr1),y        ; get next char
        sec                     ; check if char is in digit space
        sbc     #'0'            ; so subtract lower limit
        tax                     ; remember this numeric value
        cmp     #10             ; and check if upper limit is not crossed
        bcs     L8              ; done

; Multiply ptr2 (the converted value) by 10

        jsr     mul2            ; * 2

        lda     sreg+1
        pha
        lda     sreg
        pha
        lda     ptr2+1
        pha
        lda     ptr2
        pha                     ; save value

        jsr     mul2            ; * 4
        jsr     mul2            ; * 8

        clc
        pla
        adc     ptr2
        sta     ptr2
        pla
        adc     ptr2+1
        sta     ptr2+1
        pla
        adc     sreg
        sta     sreg
        pla
        adc     sreg+1
        sta     sreg+1          ; x*2 + x*8 = x*10

; Get the character back and add it

        txa                     ; restore numeric value back to accu
        clc
        adc     ptr2
        sta     ptr2
        bcc     L7
        inc     ptr2+1
        bne     L7
        inc     sreg
        bne     L7
        inc     sreg+1

; Next character

L7:     iny
        bne     L6
        inc     ptr1+1
        bne     L6

; Conversion done. Load the low 16 bit into A/X

L8:     lda     ptr2
        ldx     ptr2+1

; Negate the value if necessary, otherwise we're done

        ldy     tmp1            ; sign
        beq     L9              ; branch if positive

; Negate the 32 bit value in ptr2/sreg

        jmp     negeax

;
; Helper functions
;

mul2:   asl     ptr2
        rol     ptr2+1
        rol     sreg
        rol     sreg+1          ; * 2
L9:     rts
