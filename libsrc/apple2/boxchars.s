
.ifndef __APPLE2ENH__

        .constructor    initboxchars
        .import         machinetype

        .export         _CH_HLINE
        .export         _CH_VLINE
        .export         _CH_HLINE_SCRCODE
        .export         _CH_VLINE_SCRCODE
        .export         _CH_ULCORNER
        .export         _CH_URCORNER
        .export         _CH_LLCORNER
        .export         _CH_LRCORNER
        .export         _CH_TTEE
        .export         _CH_BTEE
        .export         _CH_LTEE
        .export         _CH_RTEE
        .export         _CH_CROSS

; User-facing characters
_CH_HLINE    = HORIZ
_CH_VLINE    = VERT
_CH_HLINE_SCRCODE    = HORIZ_SCRCODE
_CH_VLINE_SCRCODE    = VERT_SCRCODE
_CH_ULCORNER = ULCORNER
_CH_URCORNER = URCORNER
_CH_LLCORNER = LLCORNER
_CH_LRCORNER = LRCORNER
_CH_TTEE     = ULCORNER
_CH_BTEE     = LLCORNER
_CH_LTEE     = LLCORNER
_CH_RTEE     = LRCORNER
_CH_CROSS    = LLCORNER

        .segment        "ONCE"

initboxchars:
        bit     machinetype
        bvs     :+

        ; No mousetext, patch characters
        lda     #'-'
        sta     HORIZ
        ora     #$80
        sta     HORIZ_SCRCODE
        lda     #'!'
        sta     VERT
        ora     #$80
        sta     VERT_SCRCODE
        lda     #'+'
        sta     ULCORNER
        sta     URCORNER
        sta     LLCORNER
        sta     LRCORNER
:       rts

        .data

; Values that depend on the presence of MouseText chars
HORIZ:         .byte      $5F
HORIZ_SCRCODE: .byte      $5F | $80

VERT:          .byte      $5F | $80
VERT_SCRCODE:  .byte      $5F

ULCORNER:      .byte      $5F
URCORNER:      .byte      $20
LLCORNER:      .byte      $D4
LRCORNER:      .byte      $DF

.endif                ; not __APPLE2ENH__
