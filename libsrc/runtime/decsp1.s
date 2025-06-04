;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Decrement the stackpointer by 1
;

        .export         decsp1
        .importzp       spc

.proc   decsp1

        ldy     spc
        bne     @L1
        dec     spc+1
@L1:    dec     spc
        rts

.endproc





