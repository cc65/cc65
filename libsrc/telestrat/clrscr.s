    .export    _clrscr
	
    .importzp  sp
	
    .include   "telestrat.inc"

.proc _clrscr
    lda     #<SCREEN
	ldy     #>SCREEN
    sta     RES
    sty     RES+1

    ldy     #<(SCREEN+SCREEN_XSIZE*SCREEN_YSIZE)
    ldx     #>(SCREEN+SCREEN_XSIZE*SCREEN_YSIZE)
    lda     #' '
    BRK_TELEMON XFILLM
	
	
	; reset prompt position
    lda     #<(SCREEN+40)
    sta     ADSCRL
    lda     #>(SCREEN+40)
    sta     ADSCRH
	
	; reset display position
	lda     #$01
    sta     SCRY
    lda     #$02
    sta     SCRX	
    rts
.endproc	
