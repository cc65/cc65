;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Return 0 in a/x
;

        .export         return0

.proc   return0

        lda     #0
        tax
        rts

.endproc




        
