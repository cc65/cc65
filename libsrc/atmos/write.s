;
; Ullrich von Bassewitz, 2003-04-13
;
; int write (int fd, const void* buf, int count);
;
; This function is a hack!
;

        .export         _write
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3, tmp1

        .include        "atmos.inc"

.proc   _write

        sta     ptr3
        stx     ptr3+1          ; save count as result

        inx
        stx     ptr2+1
        tax
        inx
        stx     ptr2            ; save count with each byte incremented separately

        jsr     popptr1         ; get buf
        jsr     popax           ; get fd and discard
L1:     dec     ptr2
        bne     L2
        dec     ptr2+1
        beq     L9
L2:     ldy     #0
        lda     (ptr1),y
        tax
        cpx     #$0A            ; Check for \n
        bne     L3
        jsr     PRINT
        ldx     #$0D
L3:     jsr     PRINT
        inc     ptr1
        bne     L1
        inc     ptr1+1
        jmp     L1

; No error, return count

L9:     lda     ptr3
        ldx     ptr3+1
        rts

.endproc


