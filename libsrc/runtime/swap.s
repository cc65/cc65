;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: swap ax with TOS
;

        .export         swapstk
        .importzp       sp, ptr4

        .macpack        cpu

swapstk:
        sta     ptr4
        stx     ptr4+1
        ldy     #1              ; index
        lda     (sp),y
        tax
        lda     ptr4+1
        sta     (sp),y
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (sp)
        tay
        lda     ptr4
        sta     (sp)
        tya    
.else        
        dey
        lda     (sp),y
        pha
        lda     ptr4
        sta     (sp),y
        pla
.endif        
        rts                     ; whew!
