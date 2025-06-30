;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 4
;

        .export         decsp4
        .importzp       c_sp

.proc   decsp4

        lda     c_sp
        sec
        sbc     #4
        sta     c_sp
        bcc     @L1
        rts

@L1:    dec     c_sp+1
        rts

.endproc





