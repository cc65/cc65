;
; Christian Groessler, 02-Apr-2019
;

        .export         _bordercolor

        .include        "atari.inc"


_bordercolor:
        ldx     COLOR4  ; get old value
        sta     COLOR4  ; set new value
        txa
        ldx     #0      ; fix X
        rts
