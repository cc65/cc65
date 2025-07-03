;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: swap ax with TOS
;

        .export         swapstk
        .importzp       c_sp, ptr4

swapstk:
        sta     ptr4
        stx     ptr4+1
        ldy     #1              ; index
        lda     (c_sp),y
        tax
        lda     ptr4+1
        sta     (c_sp),y
.if .cap(CPU_HAS_ZPIND)
        lda     (c_sp)
        tay
        lda     ptr4
        sta     (c_sp)
        tya
.else
        dey
        lda     (c_sp),y
        pha
        lda     ptr4
        sta     (c_sp),y
        pla
.endif
        rts                     ; whew!
