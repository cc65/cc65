;
; 1998-06-11, Ullrich von Bassewitz
; 2018-05-29, Greg King
;
; char* __fastcall__ strpbrk (const char* str, const char* set);
;

        .export         _strpbrk

        .import         popax
        .importzp       ptr1, ptr2, tmp2, tmp3

_strpbrk:
        sta     ptr2            ; store set
        stx     ptr2+1
        jsr     popax
        stx     ptr1+1          ; store str's high byte
        ldx     #<$0000
        stx     ptr1
        tay                     ; use str's low byte as index

L1:     lda     (ptr1),y        ; get next char from str
        beq     L9              ; jump if done
        sta     tmp2            ; save char
        sty     tmp3            ; save index into str

        ldy     #$00
L3:     lda     (ptr2),y        ; look at each char in set
        beq     L4              ; jump if done
        cmp     tmp2
        beq     L6              ; break out of loops if something found
        iny
        bne     L3

; The character was not found in set. Increment the counter; and start over.

L4:     ldy     tmp3            ; reload index
        iny
        bne     L1
        inc     ptr1+1
        bne     L1              ; branch always

; A character was found. Return its str pointer.

L6:     ldx     ptr1+1
        lda     tmp3            ; get .Y offset
        rts

; None of the characters in set was found -- return NULL.

L9:     ;ldx     #>$0000        ; (set by prolog)
        ;lda     #<$0000        ; (set by '\0' at end of str)
        rts
