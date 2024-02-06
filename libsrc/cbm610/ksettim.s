;
; Ullrich von Bassewitz, 2003-12-19
;
; SETTIM kernal call
;

        .export         SETTIM
        .import         time : zeropage


.proc   SETTIM

        sei
        sta     time+0
        stx     time+1
        sty     time+2
        cli
        rts

.endproc

