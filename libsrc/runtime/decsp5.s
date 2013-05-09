;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 5
;

        .export         decsp5
        .importzp       sp

.proc   decsp5

        lda     sp
        sec
        sbc     #5
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc





