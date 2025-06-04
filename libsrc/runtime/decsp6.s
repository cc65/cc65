;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 6
;

        .export         decsp6
        .importzp       spc

.proc   decsp6

        lda     spc
        sec
        sbc     #6
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





