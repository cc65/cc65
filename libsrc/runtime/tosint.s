;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from long to int
;

        .export         tosint
        .import         incsp2
        .importzp       spc

        .macpack        cpu

; Convert TOS from long to int by cutting of the high 16bit

.proc   tosint

        pha
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)
.else
        ldy     #0
        lda     (spc),y          ; spc+1
.endif
        ldy     #2
        sta     (spc),y
        dey
        lda     (spc),y
        ldy     #3
        sta     (spc),y
        pla
        jmp     incsp2          ; Drop 16 bit

.endproc
