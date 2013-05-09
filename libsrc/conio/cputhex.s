;
; Ullrich von Bassewitz, 08.08.1998
;
; void cputhex8 (unsigned char val);
; void cputhex16 (unsigned val);
;

        .export         _cputhex8, _cputhex16
        .import         _cputc
        .import         __hextab


_cputhex16:
        pha                     ; Save low byte
        txa                     ; Get high byte into A
        jsr     _cputhex8       ; Output high byte
        pla                     ; Restore low byte and run into _cputhex8

_cputhex8:
        pha                     ; Save the value
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        tay
        lda     __hextab,y
        jsr     _cputc
        pla
        and     #$0F
        tay
        lda     __hextab,y
        jmp     _cputc







