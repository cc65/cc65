;
; Ullrich von Bassewitz, 29.12.1999
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: long pop
;

        .export         popeax
        .import         incsp4
        .importzp       spc, sreg

        .macpack        cpu

popeax: ldy     #3
        lda     (spc),y
        sta     sreg+1
        dey
        lda     (spc),y
        sta     sreg
        dey
        lda     (spc),y
        tax
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)
.else
        dey
        lda     (spc),y
.endif
        jmp     incsp4


