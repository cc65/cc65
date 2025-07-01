;
; Ullrich von Bassewitz, 31.05.1998
;
; Note: strspn & strcspn call internally this function and rely on
; the usage of only ptr4 here! Keep in mind when applying changes
; and check the other implementations too!
;
; size_t __fastcall__ strlen (const char* s);
;

        .export         _strlen, _strlen_ptr4
        .importzp       ptr4

_strlen:
        sta     ptr4            ; Save s
        stx     ptr4+1
_strlen_ptr4:
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

L1:     lda     (ptr4),y
        beq     L9
        iny
        bne     L1
        inc     ptr4+1
        inx
        bne     L1

L9:     tya                     ; get low byte of counter, hi's all set
        rts
