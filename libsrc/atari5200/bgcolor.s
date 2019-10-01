;
; Greg King, 10-Apr-2019
;

        .export         _bgcolor

        .include        "atari5200.inc"

.data

old_bg_index:
        .byte   COLOR_BLACK     ; see conioscreen.s for default palette

.code

_bgcolor:
        and     #$03
        tax
        ldy     COLOR0,x
        lda     old_bg_index
        sty     COLOR4          ; set new value
        stx     old_bg_index
        ldx     #0              ; fix high byte
        rts

.end
