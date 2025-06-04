;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 4
;

        .export         decsp4
        .importzp       spc

.proc   decsp4

        lda     spc
        sec
        sbc     #4
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





