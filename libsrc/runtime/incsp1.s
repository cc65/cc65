;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 1
;

        .export         incsp1
        .importzp       spc

.proc   incsp1

        inc     spc
        bne     @L1
        inc     spc+1
@L1:    rts

.endproc





