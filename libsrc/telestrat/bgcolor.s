; 2019-07-02, Jede (jede@oric.org)
;

        .export         _bgcolor
        .import         BGCOLOR
        .import         BGCOLOR_CHANGE
        .include        "telestrat.inc"

.proc _bgcolor
    cmp     BGCOLOR       ; Do we set the same color? If we don't detect it, we loose one char on the screen for each bgcolor call with the same color
    bne     out           ; Yes
    
    ldy     #$00
    sty     BGCOLOR_CHANGE

    rts
out:
    ldx     BGCOLOR       ; Get last color in order to return it
    sta     BGCOLOR

    lda     #$01          ; Notify the change color  
    sta     BGCOLOR_CHANGE  
    txa                   ; Return previous color
    rts
.endproc
