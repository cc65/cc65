;
; Ullrich von Bassewitz, 1998-08-21, 2009-02-22
;
; CC65 runtime: Pop TOS into sreg
;

        .export         popsreg
        .import         incsp2
        .importzp       c_sp, sreg

popsreg:
        pha                     ; save A
        ldy     #1
        lda     (c_sp),y        ; get hi byte
        sta     sreg+1          ; store it
.if .cap(CPU_HAS_ZPIND)
        lda     (c_sp)          ; get lo byte
.else
        dey
        lda     (c_sp),y        ; get lo byte
.endif
        sta     sreg            ; store it
        pla                     ; get A back
        jmp     incsp2          ; bump stack and return

