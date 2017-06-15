;
; Ullrich von Bassewitz, 31.05.1998
;
; Note: strspn & strcspn call internally this function and rely on
; the usage of only ptr1 here! Keep in mind when appling changes
; and check the other implementations too!
;
; int strlen (const char* s);
;

        .export         _strlen
        .importzp       ptr1

_strlen:
        sta     ptr1            ; Save s
        stx     ptr1+1
        ldx     #0              ; YX used as counter
        ldy     #0

L1:     lda     (ptr1),y
        beq     L9
        iny
        bne     L1
        inc     ptr1+1
        inx
        bne     L1

L9:     tya                     ; get low byte of counter, hi's all set
        rts
