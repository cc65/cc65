;
; Ullrich von Bassewitz, 29.12.1999
;
; CC65 runtime: Increment eax by value in Y
;

        .export         inceaxy
        .importzp       ptr4, sreg

.proc   inceaxy

        sty     ptr4
        clc
        adc     ptr4
        bcc     @L9
        inx
        bne     @L9
        inc     sreg
        bne     @L9
        inc     sreg+1
@L9:    rts

.endproc

