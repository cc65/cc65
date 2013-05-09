;
; Ullrich von Bassewitz, 11.06.1998
;
; size_t strspn (const char* s1, const char* s2);
;

        .export         _strspn
        .import         popax
        .importzp       ptr1, ptr2, tmp1, tmp2, tmp3

_strspn:
        sta     ptr2            ; Save s2
        stx     ptr2+1
        jsr     popax           ; get s1
        sta     ptr1
        stx     ptr1+1
        ldx     #0              ; low counter byte
        stx     tmp1            ; high counter byte
        ldy     #$00

L1:     lda     (ptr1),y        ; get next char from s1
        beq     L6              ; jump if done
        sta     tmp2            ; save char
        iny
        bne     L2
        inc     ptr1+1
L2:     sty     tmp3            ; save index into s1

        ldy     #0              ; get index into s2
L3:     lda     (ptr2),y        ;
        beq     L6              ; jump if done
        cmp     tmp2
        beq     L4
        iny
        bne     L3

; The character was found in s2. Increment the counter and start over

L4:     ldy     tmp3            ; reload index
        inx
        bne     L1
        inc     tmp1
        bne     L1

; The character was not found, or we reached the end of s1. Return count of
; characters

L6:     txa                     ; get low counter byte
        ldx     tmp1            ; get high counter byte
        rts






