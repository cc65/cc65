;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Optimized by Brandon Woodward
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .import         return0, return1, ppubuf_put, colors
        .export         _textcolor, _bgcolor, _bordercolor

        .include        "nes.inc"

_bordercolor    = return0

_bgcolor:
        ldy     #0
        beq     L1

_textcolor:
        ldy     #1

L1:     tax
        lda     BGCOLOR,y       ; get old value
        pha
        txa
        sta     BGCOLOR,y       ; set new value

        lda     colors,x
        ldx     #>$3F00         ; Y already has low byte of ptr
        jsr     ppubuf_put

        pla
        rts
