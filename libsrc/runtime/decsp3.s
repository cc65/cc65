;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 3
;

        .export         decsp3
        .importzp       spc

.proc   decsp3

        lda     spc
        sec
        sbc     #3
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





