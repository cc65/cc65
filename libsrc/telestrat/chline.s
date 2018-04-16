;
; jede jede@oric.org 2018-04-17
;

; void chline (unsigned char length);
;

        .export         _chline
        .include        "telestrat.inc"
        .include        "zeropage.inc"        


.proc _chline
    sta     tmp1
@loop:
    lda     #'-'            ; horizontal line screen code
    BRK_TELEMON  XWR0       ; macro send char to screen (channel 0 in telemon terms)
    dec     tmp1
    bne     @loop
    rts
.endproc

