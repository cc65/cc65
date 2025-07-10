;
; Stefan Haubenthal, 2023-01-16
;

        .export         initst

        .include        "c64.inc"

.proc   initst

        lda     #$00
        sta     STATUS
        rts

.endproc
