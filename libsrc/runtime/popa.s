;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Pop a from stack
;

        .export         popa
        .importzp       c_sp

.proc   popa

.if .cap(CPU_HAS_ZPIND)
        lda     (c_sp)
.else
        ldy     #0              ; (2)
        lda     (c_sp),y        ; (7) Read byte
.endif
        inc     c_sp            ; (12)
        beq     @L1             ; (14)
        rts                     ; (20)

@L1:    inc     c_sp+1
        rts

.endproc


