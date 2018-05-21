;                                                   
; Ullrich von Bassewitz, 11.06.1998
;
; char* strpbrk (const char* s1, const char* s2);
;

        .export         _strpbrk
        .import         popax, return0
        .importzp       ptr1, ptr2, tmp1, tmp2, tmp3

_strpbrk:
        jsr     popax           ; get s2
        sta     ptr2
        stx     ptr2+1
        jsr     popax           ; get s1
        sta     ptr1
        stx     ptr1+1
        ldy     #$00

L1:     lda     (ptr1),y        ; get next char from s1
        beq     L9              ; jump if done
        sta     tmp2            ; save char
        iny
        bne     L2
        inc     ptr1+1
L2:     sty     tmp3            ; save index into s1

        ldy     #0              ; get index into s2
L3:     lda     (ptr2),y        ;
        beq     L4              ; jump if done
        cmp     tmp2
        beq     L6
        iny
        bne     L3

; The character was not found in s2. Increment the counter and start over

L4:     ldy     tmp3            ; reload index
        inx
        bne     L1
        inc     tmp1
        bne     L1

; A character was found. Calculate a pointer to this char in s1 and return it.

L6:     ldx     ptr1+1
        lda     tmp3            ; get y offset
        clc
        adc     ptr1
        bcc     L7
        inx
L7:     rts

; None of the characters in s2 was found - return NULL

L9:     jmp     return0




