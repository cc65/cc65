;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: swap ax with TOS
;

        .export         swapstk
        .importzp       sp, ptr4

swapstk:
        sta     ptr4
        stx     ptr4+1
        ldy     #1              ; index
        lda     (sp),y
        tax
        lda     ptr4+1
        sta     (sp),y
        dey
        lda     (sp),y
        pha
        lda     ptr4
        sta     (sp),y
        pla
        rts                     ; whew!

