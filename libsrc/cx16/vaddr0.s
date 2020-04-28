;
; 2019-12-22, Greg King
;
; Set the __far__ address that VERA will use for data access.
; This is a support function for the fastcall functions vpeek() and vpoke().
;

        .export         vaddr0

        .importzp       sreg
        .importzp       tmp1
        .include        "cx16.inc"


vaddr0: stz     VERA::CTRL      ; set address for VERA's data port zero
        ldy     sreg
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        tya
        and     #$01            ; VERA ADDR_H now limited to bit 16
        sta     tmp1
        lda     VERA::ADDR+2    ; Load current register to preserve Address Increment and DECR fields
        ora     tmp1            ; incorporate high bit of address
        sta     VERA::ADDR+2
        rts
