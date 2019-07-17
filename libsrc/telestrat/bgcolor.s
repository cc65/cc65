; 2019-07-02, Jede (jede@oric.org)
;

        .export         _bgcolor
        .import         BGCOLOR

        .include        "telestrat.inc"

.proc _bgcolor
    sta     BGCOLOR
    rts
.endproc
