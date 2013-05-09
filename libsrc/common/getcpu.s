;
; Ullrich von Bassewitz, 02.04.1999
;
; unsigned char getcpu (void);
;

        .export         _getcpu

; ---------------------------------------------------------------------------
; Subroutine to detect an 816. Returns
;
;   - carry clear and 0 in A for a NMOS 6502 CPU
;   - carry set and 1 in A for some CMOS 6502 CPU
;   - carry set and 2 in A for a 65816
;
; This function uses a $1A opcode which is a INA on the 816 and ignored
; (interpreted as a NOP) on a NMOS 6502. There are several CMOS versions
; of the 6502, but all of them interpret unknown opcodes as NOP so this is
; just what we want.

.p816                           ; Enable 65816 instructions

_getcpu:
        lda     #0
        inc     a               ; .byte $1A
        cmp     #1
        bcc     @L9

; This is at least a 65C02, check for a 65816

        xba                     ; .byte $eb, put $01 in B accu
        dec     a               ; .byte $3a, A=$00 if 65C02
        xba                     ; .byte $eb, get $01 back if 65816
        inc     a               ; .byte $1a, make $01/$02
@L9:    ldx     #0              ; Load high byte of word
        rts

