;
; Ullrich von Bassewitz, 28.09.1998
;
; Write to the CRTC.
;

        .export         write_crtc, read_crtc
        .importzp       crtc, ktmp

        .include        "cbm610.inc"


; Write a value to the CRTC. The index is in Y, the value in A

.proc   write_crtc
        sta     ktmp
        lda     IndReg
        pha
        lda     #$0F
        sta     IndReg
        tya
        ldy     #$00
        sei
        sta     (crtc),y
        iny
        lda     ktmp
        sta     (crtc),y
        cli
        pla
        sta     IndReg
        lda     ktmp
        rts
.endproc


.proc   read_crtc
        sty     ktmp
        lda     IndReg
        pha
        lda     #$0F
        sta     IndReg
        lda     ktmp
        ldy     #$00
        sei
        sta     (crtc),y
        iny
        lda     (crtc),y
        cli
        tay
        pla
        sta     IndReg
        tya
        ldy     ktmp
        rts
.endproc


