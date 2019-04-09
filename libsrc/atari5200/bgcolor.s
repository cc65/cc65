;
; Christian Groessler, 05-Apr-2019
;

        .export         _bgcolor

        .include        "atari5200.inc"

        .constructor    init_old_bgcolor

.bss

old_bg_color:
        .res    1

.code

_bgcolor:
        and     #3
        tax
        lda     COLOR0,x
        ldx     old_bg_color
        sta     COLOR4                  ; set new value
        sta     old_bg_color
        txa
        ldx     #0                      ; fix X
        rts

.segment        "ONCE"

init_old_bgcolor:
        lda     COLOR0+3                ; see also conioscreen.s for initialization
        sta     old_bg_color
        rts

        .end
