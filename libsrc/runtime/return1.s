;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Return 1 in a/x
;

        .export         return1

.proc   return1

        ldx     #0
        lda     #1
        rts

.endproc




        
