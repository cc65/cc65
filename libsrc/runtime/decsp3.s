;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 3
;

        .export         decsp3
        .importzp       sp

.proc   decsp3

        lda     sp
        sec
        sbc     #3
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc





