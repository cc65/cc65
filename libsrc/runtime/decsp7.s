;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 7
;

        .export         decsp7
        .importzp       sp

.proc   decsp7

        lda     sp
        sec
        sbc     #7
        sta     sp
        bcc     @L1
        rts

@L1:    dec     sp+1
        rts

.endproc



