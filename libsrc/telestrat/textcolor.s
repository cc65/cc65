; 2019-07-02, Jede (jede@oric.org)
;

        .export         _textcolor
        .import         CHARCOLOR
        .import         CHARCOLOR_CHANGE
        .include        "telestrat.inc"

.proc _textcolor
    cmp     CHARCOLOR       ; Do we set the same color? if we don't detect it, we loose one char on the screen for each textcolor call with the same color
    bne     out             ; yes
    lda     #$00
    sta     CHARCOLOR_CHANGE
    
    lda     CHARCOLOR       ; return last color

    rts
out:
    ldx     CHARCOLOR       ; get last color in order to return it
    sta     CHARCOLOR

    lda     #$01
    sta     CHARCOLOR_CHANGE
    txa                     ; return previous color
    rts
.endproc


