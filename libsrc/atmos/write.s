;
; Ullrich von Bassewitz, 2003-04-13
;
; int write (int fd, const void* buf, int count);
;
; This function is a hack!
;

        .export         _write
        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1

        .include        "atmos.inc"

.proc   _write

        sta     ptr3
        stx     ptr3+1          ; save count as result

        eor     #$FF
        sta     ptr2
        txa
        eor     #$FF
        sta     ptr2+1          ; Remember -count-1

        jsr     popax           ; get buf
        sta     ptr1
        stx     ptr1+1
        jsr     popax           ; get fd and discard
L1:     inc     ptr2
        bne     L2
        inc     ptr2+1
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


