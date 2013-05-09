;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push a extended with FF onto the stack
;

        .export         pushaFF
        .import         pushax

; Beware: The optimizer knows about this function!

.proc   pushaFF

        ldx     #$FF
        jmp     pushax

.endproc


                      
