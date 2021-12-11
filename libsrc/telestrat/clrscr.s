;
; jede jede@oric.org 2017-02-25
;

    .export    _clrscr
    .import    OLD_CHARCOLOR, OLD_BGCOLOR, BGCOLOR, CHARCOLOR
    
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
    lda     #<SCREEN
    sta     ADSCR
    lda     #>SCREEN
    sta     ADSCR+1

    lda     #$00
    sta     SCRDY

    ; reset display position
    ldx     #$00
    stx     SCRY
    stx     SCRX
    
    stx     OLD_BGCOLOR                             ; Black
    stx     BGCOLOR
    
    ldx     #$07                                    ; White
    stx     OLD_CHARCOLOR
    stx     CHARCOLOR
    
    rts
.endproc
