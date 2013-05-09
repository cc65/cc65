;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 8
;

        .export         decsp8
        .importzp       sp

.proc   decsp8

        lda     sp
        sec
        sbc     #8
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc





