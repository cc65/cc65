;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 2. For performance reasons,
;               this module also contains the popax function.

        .export         popax, incsp2
        .importzp       c_sp

; Pop a/x from stack. This function will run directly into incsp2

.proc   popax

        ldy     #1
        lda     (c_sp),y        ; get hi byte
        tax                     ; into x
.if .cap(CPU_HAS_ZPIND)
        lda     (c_sp)          ; get lo byte
.else
        dey
        lda     (c_sp),y        ; get lo byte
.endif

.endproc



.proc   incsp2

        inc     c_sp            ; 5
        beq     @L1             ; 2
        inc     c_sp            ; 5
        beq     @L2             ; 2
        rts

@L1:    inc     c_sp            ; 5
@L2:    inc     c_sp+1          ; 5
        rts

.endproc





