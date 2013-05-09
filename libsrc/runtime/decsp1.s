;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 1
;

        .export         decsp1
        .importzp       sp

.proc   decsp1

        ldy     sp
        bne     @L1
        dec     sp+1
@L1:    dec     sp
        rts

.endproc




        
