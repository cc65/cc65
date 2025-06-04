;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: swap ax with TOS
;

        .export         swapstk
        .importzp       spc, ptr4

        .macpack        cpu

swapstk:
        sta     ptr4
        stx     ptr4+1
        ldy     #1              ; index
        lda     (spc),y
        tax
        lda     ptr4+1
        sta     (spc),y
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (spc)
        tay
        lda     ptr4
        sta     (spc)
        tya
.else
        dey
        lda     (spc),y
        pha
        lda     ptr4
        sta     (spc),y
        pla
.endif
        rts                     ; whew!
