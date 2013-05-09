;
; Christian Groessler, 27-Dec-2002
;

        .export         _textcolor, _bgcolor, _bordercolor
        .import         return1

        .include        "atari.inc"


_textcolor      =       return1

_bgcolor:
        ldx     COLOR2  ; get old value
        sta     COLOR2  ; set new value
        and     #$0e
        cmp     #8
        bcs     bright
        lda     #$0e
        .byte   $2c     ; bit opcode, eats the next 2 bytes
bright: lda     #0
        sta     COLOR1
        txa
        ldx     #0      ; fix X
        rts


_bordercolor:
        ldx     COLOR4  ; get old value
        sta     COLOR4  ; set new value
        txa
        ldx     #0      ; fix X
        rts

