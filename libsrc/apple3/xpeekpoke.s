;
; Robert Justice, 2026
;
; unsigned char __fastcall__ xpeek (unsigned char xbyte, unsigned int addr);
; void __fastcall__ xpoke (unsigned char xbyte, unsigned int addr, unsigned char val);
;


        .export         _xpeek, _xpoke
        .import         popptr1, popa

        .include        "zeropage.inc"
        .include        "apple3.inc"

_xpeek:
        sta     ptr1            ; set address
        stx     ptr1+1
        jsr     popa            ; get xbyte
        sta     ptr1+INTEXTPG   ; set interp ext page xbyte
        ldy     #0
        lda     (ptr1),Y        ; read value using extended addressing
        sty     ptr1+INTEXTPG   ; disable extended addressing
        ldx     #>$0000
        rts

_xpoke:
        pha                     ; save value
        jsr     popptr1         ; set address
        jsr     popa            ; get xbyte
        sta     ptr1+INTEXTPG   ; set interp ext page xbyte
        ldy     #0
        pla
        sta     (ptr1),Y
        sty     ptr1+INTEXTPG   ; disable extended addressing
        rts
