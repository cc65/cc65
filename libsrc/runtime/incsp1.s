;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 1
;

        .export         incsp1
        .importzp       sp

.proc   incsp1

        inc     sp
        bne     @L1
        inc     sp+1
@L1:    rts

.endproc




        
