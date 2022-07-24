;
; Ullrich von Bassewitz, 03.06.1999
;
; unsigned cbm_k_iobase (void);
;

        .include        "cbm.inc"

        .export         _cbm_k_iobase

_cbm_k_iobase:
        jsr     IOBASE
        txa
        pha
        tya
        tax
        pla
        rts
