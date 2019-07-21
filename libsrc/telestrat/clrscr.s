;
; jede jede@oric.org 2017-02-25
;

    .export    _clrscr
    .import    OLD_CHARCOLOR, OLD_BGCOLOR
    
    .include   "telestrat.inc"

.proc _clrscr
    ; Switch to text mode
    BRK_TELEMON(XTEXT)

    lda     #<SCREEN                                ; Get position screen
    ldy     #>SCREEN
    sta     RES
    sty     RES+1

    ldy     #<(SCREEN+SCREEN_XSIZE*SCREEN_YSIZE)
    ldx     #>(SCREEN+SCREEN_XSIZE*SCREEN_YSIZE)
    lda     #' '
    BRK_TELEMON XFILLM                              ; Calls XFILLM : it fills A value from RES address and size of X and Y value


    ; reset prompt position
    lda     #<(SCREEN+40)
    sta     ADSCRL
    lda     #>(SCREEN+40)
    sta     ADSCRH

    ; reset display position
    ldx     #$01
    stx     SCRY
    dex
    stx     SCRX
    
    ; At this step X is equal to $00
    dex
    ; At this step X is equal to $FF
    stx     OLD_BGCOLOR
    stx     OLD_CHARCOLOR

    rts
.endproc
