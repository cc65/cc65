; 2019-07-02, Jede (jede@oric.org)
;

        .export         _textcolor
        .import         CHARCOLOR, OLD_CHARCOLOR
        .include        "telestrat.inc"

.proc _textcolor
    ldx     CHARCOLOR     ; Get previous color
    sta     CHARCOLOR
    stx     OLD_CHARCOLOR
    txa                   ; Return previous color
    rts
.endproc
