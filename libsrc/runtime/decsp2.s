;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 2
;

        .export         decsp2
        .importzp       sp

.proc   decsp2

        lda     sp
        sec
        sbc     #2
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc





