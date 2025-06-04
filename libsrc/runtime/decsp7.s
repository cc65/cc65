;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 7
;

        .export         decsp7
        .importzp       spc

.proc   decsp7

        lda     spc
        sec
        sbc     #7
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc



