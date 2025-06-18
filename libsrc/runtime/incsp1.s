;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 1
;

        .export         incsp1
        .importzp       c_sp

.proc   incsp1

        inc     c_sp
        bne     @L1
        inc     c_sp+1
@L1:    rts

.endproc





