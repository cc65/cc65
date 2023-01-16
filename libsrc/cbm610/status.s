;
; Stefan Haubenthal, 2023-01-16
;

        .export         initst

        .include        "extzp.inc"
        .include        "cbm610.inc" 

.proc   initst

        ldx     IndReg
        ldy     #$0F
        sty     IndReg          ; Switch to the system bank
        ldy     #STATUS
        lda     #$00            ; Initialize value
        sta     (sysp0),y
        stx     IndReg
        rts

.endproc
