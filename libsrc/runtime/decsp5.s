;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 5
;

        .export         decsp5
        .importzp       spc

.proc   decsp5

        lda     spc
        sec
        sbc     #5
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





