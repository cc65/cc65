;; _read.s
        
        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1

        .import         _serial_getc
        .export         _read
        
        .include "c39.inc"
                
.proc _read
        sta     ptr3
        stx     ptr3+1          ; save count as result, was store a!

        eor     #$FF
        sta     ptr2
        txa
        eor     #$FF
        sta     ptr2+1          ; Remember -count-1

        jsr     popax           ; get buf
        sta     ptr1
        stx     ptr1+1
        jsr     popax           ; get fd and discard
L1:
        inc     ptr2
        bne     L2
        inc     ptr2+1
        beq     L9
L2:
        jsr     _serial_getc

        ;; C39 FIX
        ;ldy     #0
        ;sta    (ptr1),y
        ldx     #0
        sta     (ptr1),x
        
        inc     ptr1
        bne     L1
        inc     ptr1+1
        jmp     L1

; No error, return count

L9:     lda     ptr3
        ldx     ptr3+1
        rts
.endproc
