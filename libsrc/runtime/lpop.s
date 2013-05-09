;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: long pop
;

        .export         popeax
        .import         incsp4
        .importzp       sp, sreg


popeax: ldy     #3
        lda     (sp),y
        sta     sreg+1
        dey
        lda     (sp),y
        sta     sreg
        dey
        lda     (sp),y
        tax
        dey
        lda     (sp),y
        jmp     incsp4


