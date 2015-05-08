;; _read.s
        
        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1
        
        .export         _read

SERIAL_BUF    = $38
SERIAL_STATUS = $3C
        
.define BIT_SIN_BUFFER_FULL    0
.define BIT_SIN_OVERRUN_ERROR  1
.define BIT_SIN_PARITY_ERROR   2
.define BIT_SIN_FRAMING_ERROR  3
.define BIT_SIN_BREAK_DETECTED 4
.define BIT_SOUT_BUFFER_EMPTY  5
.define BIT_SOUT_UNDERRUN      6
.define BIT_SIN_PARIITY_BIT    7
        
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

.proc _serial_getc
L1:     
        bbr #BIT_SIN_BUFFER_FULL, SERIAL_STATUS, _serial_getc
        lda SERIAL_BUF
        rts
.endproc
        
.proc _serial_getc_echo
L1:     
        bbr #BIT_SIN_BUFFER_FULL, SERIAL_STATUS, L1
        lda SERIAL_BUF
        
        ;; echo
L2:     
        bbr #BIT_SOUT_BUFFER_EMPTY, SERIAL_STATUS, L2
        sta SERIAL_BUF
        rts
.endproc
