;
; 2019-10-22, Greg King
;
; unsigned char fastcall vpeek (unsigned long addr);
; /* Get a byte from a location in VERA's internal address space. */
;

        .export         _vpeek

        .import         vset
        .include        "cx16.inc"


_vpeek: php                     ; (vset blocks interrupts)
        jsr     vset            ; put VERA's address
        ldx     #>$0000
        lda     VERA::DATA0     ; read VERA port zero
        plp
        rts
