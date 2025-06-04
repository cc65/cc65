;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Pop a from stack
;

        .export         popa
        .importzp       spc

        .macpack        cpu

.proc   popa

.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)
.else
        ldy     #0              ; (2)
        lda     (spc),y          ; (7) Read byte
.endif
        inc     spc              ; (12)
        beq     @L1             ; (14)
        rts                     ; (20)

@L1:    inc     spc+1
        rts

.endproc


