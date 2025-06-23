;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 7
;

        .export         decsp7
        .importzp       c_sp

.proc   decsp7

        lda     c_sp
        sec
        sbc     #7
        sta     c_sp
        bcc     @L1
        rts

@L1:    dec     c_sp+1
        rts

.endproc



