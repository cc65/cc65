;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 8
;

        .export         decsp8
        .importzp       spc

.proc   decsp8

        lda     spc
        sec
        sbc     #8
        sta     spc
        bcc     @L1
        rts

@L1:    dec     spc+1
        rts

.endproc





