;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned cbm_k_iobase (void);
;

        .export         _cbm_k_iobase
        .import         IOBASE

_cbm_k_iobase:                   
        jsr     IOBASE
        txa
        pha
        tya
        tax
        pla
        rts
