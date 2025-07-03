;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: xor on longs
;

        .export         tosxor0ax, tosxoreax
        .import         addysp1
        .importzp       c_sp, sreg, tmp1

tosxor0ax:
.if .cap(CPU_HAS_STZ)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif

tosxoreax:
.if .cap(CPU_HAS_ZPIND)
        eor     (c_sp)          ; byte 0
        ldy     #1
.else
        ldy     #0
        eor     (c_sp),y        ; byte 0
        iny
.endif
        sta     tmp1
        txa
        eor     (c_sp),y        ; byte 1
        tax
        iny
        lda     sreg
        eor     (c_sp),y        ; byte 2
        sta     sreg
        iny
        lda     sreg+1
        eor     (c_sp),y        ; byte 3
        sta     sreg+1

        lda     tmp1
        jmp     addysp1



