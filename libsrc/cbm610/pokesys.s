;
; Ullrich von Bassewitz, 29.09.1998
;
; void pokebsys (unsigned Addr, unsigned char Val);
; void pokewsys (unsigned Addr, unsigned Val);

        .export         _pokebsys, _pokewsys
        .import         popsreg
        .importzp       sreg, tmp1

        .include        "cbm610.inc"


_pokebsys:
        jsr     popsreg         ; Get the address
        ldx     IndReg
        ldy     #$0F
        sty     IndReg          ; Switch to the system bank
        ldy     #$00
        sta     (sreg),y
        stx     IndReg
        rts

_pokewsys:
        stx     tmp1            ; Save high byte
        jsr     popsreg         ; Get the address
        ldx     IndReg
        ldy     #$0F
        sty     IndReg          ; Switch to the system bank
        ldy     #$00
        sta     (sreg),y
        iny
        lda     tmp1
        sta     (sreg),y
        stx     IndReg
        rts


