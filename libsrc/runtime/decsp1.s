;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 1
;

        .export         decsp1
        .importzp       c_sp

.proc   decsp1

        ldy     c_sp
        bne     @L1
        dec     c_sp+1
@L1:    dec     c_sp
        rts

.endproc





