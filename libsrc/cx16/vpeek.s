;
; 2019-12-22, Greg King
;
; unsigned char fastcall vpeek (unsigned long addr);
; /* Get a byte from a location in VERA's internal address space. */
;

        .export         _vpeek

        .import         vaddr0
        .include        "cx16.inc"


_vpeek: jsr     vaddr0          ; put VERA's address
        ldx     #>$0000
        lda     VERA::DATA0     ; read VERA port zero
        rts
