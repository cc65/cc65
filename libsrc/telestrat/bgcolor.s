; 2019-07-02, Jede (jede@oric.org)
;

        .export         _bgcolor
        .import         BGCOLOR
        .import         BGCOLOR_CHANGE
        .include        "telestrat.inc"

.proc _bgcolor
    cmp     BGCOLOR       ; Do we set the same color? if we don't detect it, we loose one char on the screen for each textcolor call with the same color
    bne     out           ; Yes
    lda     #$00
    sta     BGCOLOR_CHANGE
    
    lda     BGCOLOR       ; Return last color

    rts
out:
    ldx     BGCOLOR       ; Get last color in order to return it
    sta     BGCOLOR

    lda     #$01          ; Notify the change color  
    sta     BGCOLOR_CHANGE  
    txa                   ; Return previous color
    rts
.endproc


