;
; Ullrich von Bassewitz, 31.05.1998
;
; Note: strspn & strcspn call internally this function and rely on
; the usage of only ptr2 here! Keep in mind when appling changes
; and check the other implementations too!
;
; size_t __fastcall__ strlen (const char* s);
;

        .export         _strlen
        .importzp       ptr2
        .macpack        cpu

_strlen:
        sta     ptr2            ; Save s
        stx     ptr2+1
.if (.cpu .bitand ::CPU_ISET_HUC6280)
        clx
        cly
.else
        ldx     #0              ; YX used as counter
.if (.cpu .bitand ::CPU_ISET_65816)
        txy
.else
        ldy     #0
.endif
.endif

L1:     lda     (ptr2),y
        beq     L9
        iny
        bne     L1
        inc     ptr2+1
        inx
        bne     L1

L9:     tya                     ; get low byte of counter, hi's all set
        rts
