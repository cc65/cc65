    .export    _clrscr
	
    .import    addysp,popax
	
    .importzp  sp,tmp2,tmp3,tmp1
	
	
    .include   "telestrat.inc"
    .include   "errno.inc"
    .include   "fcntl.inc"	

.proc _clrscr
    lda     #<SCREEN
    sta     RES
    lda     #>SCREEN
    sta     RES+1

    lda     #<(SCREEN+40*28)
    sta     RESB
    lda     #>(SCREEN+40*28)
    sta     RESB+1

    lda     #' '
    BRK_TELEMON XFILLM
.endproc	
