;
; Colin Leroy-Mira and Oliver Schmidt, 26.05.2025
;
; Initialize box-drawing characters according to
; MouseText availability
;

.ifndef __APPLE2ENH__

        .constructor    initboxchars
        .import         machinetype

        .export         _CH_HLINE
        .export         _CH_VLINE
        .export         _CH_ULCORNER
        .export         _CH_URCORNER
        .export         _CH_LLCORNER
        .export         _CH_LRCORNER
        .export         _CH_TTEE
        .export         _CH_BTEE
        .export         _CH_LTEE
        .export         _CH_RTEE
        .export         _CH_CROSS

        .segment        "ONCE"

initboxchars:
        bit     machinetype   ; IIe enhanced or newer?
        bvs     out

        ldx     #NUM_BOXCHARS  ; No mousetext, patch characters
:       lda     std_boxchars,x
        sta     boxchars,x
        dex
        bpl     :-

out:    rts

; Replacement chars for when MouseText is not available
std_boxchars:   .byte   '!'
                .byte   '-'
                .byte   '+'
                .byte   '+'
                .byte   '+'
                .byte   '+'

        .data

; MouseText-based box characters
boxchars:
VERT:           .byte   $DF
HORIZ:          .byte   $5F
ULCORNER:       .byte   $5F
URCORNER:       .byte   $20
LLCORNER:       .byte   $D4
LRCORNER:       .byte   $DF

NUM_BOXCHARS          = *-boxchars

; exported symbols, referencing our 6 bytes
_CH_HLINE             = HORIZ
_CH_VLINE             = VERT
_CH_ULCORNER          = ULCORNER
_CH_URCORNER          = URCORNER
_CH_LLCORNER          = LLCORNER
_CH_LRCORNER          = LRCORNER
_CH_TTEE              = ULCORNER
_CH_BTEE              = LLCORNER
_CH_LTEE              = LLCORNER
_CH_RTEE              = LRCORNER
_CH_CROSS             = LLCORNER

.endif                ; not __APPLE2ENH__
