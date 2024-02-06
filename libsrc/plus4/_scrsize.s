;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize

; We will return the values directly instead of banking in the ROM and calling
; SCREEN which is a lot more overhead in code size and CPU cycles.

.proc   screensize

        ldx     #40
        ldy     #25
        rts

.endproc



