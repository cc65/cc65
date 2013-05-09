;
; Ullrich von Bassewitz, 2003-12-21
;
; udtim routine for the 610. We will not check for the stop key here, since
; C programs will not use it.
;                                  

        .export         UDTIM
        .import         time: zp

.proc   UDTIM

        inc     time
        bne     L9
        inc     time+1
        bne     L9
        inc     time+2
        bne     L9
        inc     time+3
L9:     rts

.endproc

