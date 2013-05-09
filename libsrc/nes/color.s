;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export         _textcolor, _bgcolor, _bordercolor
        .import         return0, ppubuf_put

        .include        "nes.inc"

_textcolor      = return0
_bordercolor    = return0

.proc   _bgcolor

        tax
        lda     BGCOLOR         ; get old value
        stx     BGCOLOR         ; set new value
        pha

        lda     colors,x
        pha
        ldy     #$3F
        ldx     #0
        jsr     ppubuf_put
        pla
        pha
        ldy     #$3F
        ldx     #4
        jsr     ppubuf_put
        pla
        pha
        ldy     #$3F
        ldx     #8
        jsr     ppubuf_put
        pla
        ldy     #$3F
        ldx     #12
        jsr     ppubuf_put

        pla
        rts

.endproc

.rodata

colors: .byte $0f       ; 0 black
        .byte $3d       ; 1 white
        .byte $04       ; 2 red
        .byte $3b       ; 3 cyan
        .byte $14       ; 4 violett
        .byte $1a       ; 5 green
        .byte $01       ; 6 blue
        .byte $38       ; 7 yellow
        .byte $18       ; 8 orange
        .byte $08       ; 9 brown
        .byte $35       ; a light red
        .byte $2d       ; b dark grey
        .byte $10       ; c middle grey
        .byte $2b       ; d light green
        .byte $22       ; e light blue
        .byte $3d       ; f light gray

