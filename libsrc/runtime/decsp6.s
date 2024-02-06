;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 6
;

        .export         decsp6
        .importzp       sp

.proc   decsp6

        lda     sp
        sec
        sbc     #6
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc





