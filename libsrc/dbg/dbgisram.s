;
; Ullrich von Bassewitz, 10.08.1998
;
; int DbgIsRAM (unsigned Addr);
;

        .export         _DbgIsRAM
        .import         popax, return0, return1
        .importzp       ptr1

_DbgIsRAM:
        sta     ptr1            ; Store the address
        stx     ptr1+1

        ldy     #0
        php                     ; Save I flag
        sei                     ; Disable interrupts

        lda     (ptr1),y        ; Get old value
        pha                     ; ...and save it

        ldx     #3
L1:     lda     TestVal,x
        jsr     CheckCell
        bne     L2
        dex
        bpl     L1

; This seems to be RAM

        pla
        sta     (ptr1),y        ; Restore old value
        plp                     ; Restore old I flag
        jmp     return1

; No RAM at this address

L2:     pla
        sta     (ptr1),y        ; Restore old value
        plp                     ; Restore old I flag
        jmp     return0

; Check one memory cell

CheckCell:
        sta     (ptr1),y
        cmp     (ptr1),y        ; Could we write it?
        rts


.rodata
TestVal:
        .byte   $55, $AA, $33, $CC


