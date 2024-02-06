;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Return -1 in a/x
;

        .export         returnFFFF

.proc   returnFFFF

        lda     #$FF
        tax
        rts

.endproc
