;
; 2019-12-22, Greg King
;
; void fastcall vpoke (unsigned char data, unsigned long addr);
; /* Put a byte into a location in VERA's internal address space.
; ** (addr is second instead of first for the sake of code efficiency.)
; */
;

        .export         _vpoke

        .import         vaddr0, popa
        .include        "cx16.inc"


_vpoke: jsr     vaddr0          ; put VERA's address
        jsr     popa
        sta     VERA::DATA0     ; write data to VERA port zero
        rts
