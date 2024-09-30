;
; Ullrich von Bassewitz, 31.05.1998
;
; char* strcpy (char* dest, const char* src);
;

        .export         _strcpy
        .import         popax
        .importzp       ptr1, ptr2

_strcpy:
        sta     ptr1            ; Save src
        stx     ptr1+1
        jsr     popax           ; Get dest
        sta     ptr2
        stx     ptr2+1
        ldy     #$00

L1:     lda     (ptr1),y
        sta     (ptr2),y
        beq     L9
        iny
        bne     L1
        inc     ptr1+1
        inc     ptr2+1
        bne     L1

L9:     lda     ptr2            ; X still contains dest's original high byte

        ; On exit, we want AX to be dest (as this is what strcpy returns).
        ; We also want (ptr2),y to still point to dest's terminator, as this
        ; is used by stpcpy().
        rts
