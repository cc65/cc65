;
; Ullrich von Bassewitz, 31.05.1998
;
; int strcmp (const char* s1, const char* s2);
;

        .export         _strcmp
        .import         popax
        .importzp       ptr1, ptr2

_strcmp:
        sta     ptr2            ; Save s2
        stx     ptr2+1
        jsr     popax           ; Get s1
        sta     ptr1
        stx     ptr1+1
        ldy     #0

loop:   lda     (ptr1),y
        cmp     (ptr2),y
        bne     L1
        tax                     ; end of strings?
        beq     L3
        iny
        bne     loop
        inc     ptr1+1
        inc     ptr2+1
        bne     loop

L1:     bcs     L2
        ldx     #$FF
        rts

L2:     ldx     #$01
L3:     rts
