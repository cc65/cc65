
        .export _cpeekc

        .import ppubuf_waitempty

        .include	"nes.inc"

        .segment "CODE"

_cpeekc:

        ; wait until all console data has been written
        jsr ppubuf_waitempty

        ldy SCREEN_PTR+1
        ldx SCREEN_PTR

; waiting for vblank is incredibly slow ://
vwait:
;       lda $2002    ;wait
;       bpl vwait

        lda #0
        sty $2006
        stx $2006
        ldy $2007    ; first read is invalid
        ldy $2007    ; get data
        sta $2006
        sta $2006

        tya
        and #$7f ; ?!?!
        rts

