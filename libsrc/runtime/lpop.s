;
; Ullrich von Bassewitz, 29.12.1999
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: long pop
;

        .export         popeax
        .import         incsp4
        .importzp       c_sp, sreg

        .macpack        cpu

popeax: ldy     #3
        lda     (c_sp),y
        sta     sreg+1
        dey
        lda     (c_sp),y
        sta     sreg
        dey
        lda     (c_sp),y
        tax
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (c_sp)
.else
        dey
        lda     (c_sp),y
.endif
        jmp     incsp4


