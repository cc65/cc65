;
; Ullrich von Bassewitz, 2003-12-19
;
; IOBASE kernal call
;

        .export         IOBASE
        .import         cia : zeropage



.proc   IOBASE

        ldx     cia
        ldy     cia+1
        rts

.endproc

