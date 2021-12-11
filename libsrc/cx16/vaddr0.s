;
; 2019-12-22, Greg King
;
; Set the __far__ address that VERA will use for data access.
; This is a support function for the fastcall functions vpeek() and vpoke().
;

        .export         vaddr0

        .importzp       sreg
        .include        "cx16.inc"


vaddr0: stz     VERA::CTRL      ; set address for VERA's data port zero
        ldy     sreg
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        sty     VERA::ADDR+2
        rts
