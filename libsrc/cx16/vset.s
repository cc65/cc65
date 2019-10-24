;
; 2019-10-22, Greg King
;
; Set the __far__ address that VERA will use for data access.
; This is a support function for the fastcall functions vpeek() and vpoke().
;

        .export         vset

        .importzp       sreg
        .include        "cx16.inc"


vset:   ldy     sreg
        sei                     ; don't let interrupt handlers interfere
        stz     VERA::CTRL      ; set address for VERA's data port zero
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        sty     VERA::ADDR+2
        rts
