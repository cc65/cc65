;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 2. For performance reasons,
;               this module also contains the popax function.

        .export         popax, incsp2
        .importzp       spc

        .macpack        cpu

; Pop a/x from stack. This function will run directly into incsp2

.proc   popax

        ldy     #1
        lda     (spc),y          ; get hi byte
        tax                     ; into x
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)            ; get lo byte
.else
        dey
        lda     (spc),y          ; get lo byte
.endif

.endproc



.proc   incsp2

        inc     spc              ; 5
        beq     @L1             ; 2
        inc     spc              ; 5
        beq     @L2             ; 2
        rts

@L1:    inc     spc              ; 5
@L2:    inc     spc+1            ; 5
        rts

.endproc





