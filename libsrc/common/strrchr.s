;
; Ullrich von Bassewitz, 31.05.1998
;
; char* strrchr (const char* s, int c);
;

        .export         _strrchr
        .import         popax
        .importzp       ptr1, ptr2, tmp1

_strrchr:
        sta     tmp1            ; Save c
        jsr     popax           ; get s
        sta     ptr1
        stx     ptr1+1
        lda     #0              ; function result = NULL
        sta     ptr2
        sta     ptr2+1
        tay

L1:     lda     (ptr1),y        ; get next char
        beq     L3              ; jump if end of string
        cmp     tmp1            ; found?
        bne     L2              ; jump if no

; Remember a pointer to the character

        tya
        clc
        adc     ptr1
        sta     ptr2
        lda     ptr1+1
        adc     #$00
        sta     ptr2+1

; Next char

L2:     iny
        bne     L1
        inc     ptr1+1
        bne     L1              ; jump always

; Return the pointer to the last occurrence

L3:     lda     ptr2
        ldx     ptr2+1
        rts
