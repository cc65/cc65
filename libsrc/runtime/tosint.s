;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert tos from long to int
;

        .export         tosint
        .import         incsp2
        .importzp       c_sp

        .macpack        cpu

; Convert TOS from long to int by cutting of the high 16bit

.proc   tosint

        pha
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (c_sp)
.else
        ldy     #0
        lda     (c_sp),y          ; c_sp+1
.endif
        ldy     #2
        sta     (c_sp),y
        dey
        lda     (c_sp),y
        ldy     #3
        sta     (c_sp),y
        pla
        jmp     incsp2          ; Drop 16 bit

.endproc
