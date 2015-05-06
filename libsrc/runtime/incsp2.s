;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 2. For performance reasons,
;               this modules does also contain the popax function.

        .export         popax, incsp2
        .importzp       sp

        .macpack        cpu

; Pop a/x from stack. This function will run directly into incsp2

.proc   popax

        ldy     #1
        lda     (sp),y          ; get hi byte
        tax                     ; into x
.if (.cpu .bitand ::CPU_ISET_65SC02) .or (.cpu .bitand ::CPU_ISET_C39_EMUL)
        lda     (sp)            ; get lo byte
.else
        dey
        lda     (sp),y          ; get lo byte
.endif

.endproc



.proc   incsp2

        inc     sp              ; 5
        beq     @L1             ; 2
        inc     sp              ; 5
        beq     @L2             ; 2
        rts

@L1:    inc     sp              ; 5
@L2:    inc     sp+1            ; 5
        rts

.endproc





