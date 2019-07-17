; 2019-07-02, Jede (jede@oric.org)
;

        .export         _textcolor
        .import         CHARCOLOR
        .include        "telestrat.inc"

.proc _textcolor
    sta     CHARCOLOR
    rts
.endproc
