;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 2
;

        .export         decsp2
        .importzp       spc

.proc   decsp2

        lda     spc
        sec
        sbc     #2
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





