;
; Ullrich von Bassewitz, 29.12.1999
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: long pop
;

        .export         popeax
        .import         incsp4
        .importzp       sp, sreg

        .macpack        cpu

popeax: ldy     #3
        lda     (sp),y
        sta     sreg+1
        dey
        lda     (sp),y
        sta     sreg
        dey
        lda     (sp),y
        tax
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (sp)
.else        
        dey
        lda     (sp),y
.endif
        jmp     incsp4


