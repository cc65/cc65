
    .include	"atmos.inc"

    .export _cpeekchar

_cpeekchar:

    ldy CURS_Y
    ldx ScrTabLo,y
    stx @l+1
    ldx ScrTabHi,y
    stx @l+2
    ldx CURS_X
@l:
    lda $bb80,x
;;  inc COORDX_TEXT
    ldx #0
    rts

    ; FIXME: is that table available elsewhere?
.rodata
ScrTabLo:
    .repeat 28, Line
        .byte   <(SCREEN + Line * 40)
    .endrep

ScrTabHi:
    .repeat 28, Line
        .byte   >(SCREEN + Line * 40)
    .endrep

