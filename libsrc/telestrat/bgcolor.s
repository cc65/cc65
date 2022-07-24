; 2019-07-02, Jede (jede@oric.org)
;

        .export         _bgcolor
        .import         BGCOLOR

        .include        "telestrat.inc"

.proc _bgcolor
    ldx     BGCOLOR     ; Get previous color
    sta     BGCOLOR
    txa                 ; Return previous color
    rts
.endproc
