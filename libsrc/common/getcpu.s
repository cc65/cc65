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
;   - carry set and 1 in A for a 65c02
;   - carry set and 2 in A for a 65816
;   - carry set and 3 in A for a 4510
;   - carry set and 4 in A for a 65sc02
;   - carry set and 5 in A for a 65ce02
;
; This function uses a $1A opcode which is a INA on the 816 and ignored
; (interpreted as a NOP) on a NMOS 6502. There are several CMOS versions
; of the 6502, but all of them interpret unknown opcodes as NOP so this is
; just what we want.

.p816                           ; Enable 65816 instructions

_getcpu:
        lda     #0
        inc     a               ; .byte $1A ; nop on nmos, inc on every cmos
        cmp     #1
        bcc     @L9

; This is at least a 65C02, check for a 65ce02/4510

        .byte   $42,$ea         ; neg on 65ce02/4510, nop #$ea on 65c02, wdm $ea on 65816
        cmp     #1
        beq     @L6

; This is at least a 65ce02, check for 4510

        lda     #5              ; CPU_65CE02 constant
        .byte   $5c             ; map on 4510, aug on 65ce02 (acts like 4 byte nop)
        lda     #3              ; CPU_4510 constant
        nop
        bne     @L9

; Check for 65816/65802
@L6:    xba                     ; .byte $eb, put $01 in B accu (nop on 65c02/65sc02)
        dec     a               ; .byte $3a, A=$00
        xba                     ; .byte $eb, A=$01 if 65816/65802 and A=$00 if 65c02/65sc02
        inc     a               ; .byte $1a, A=$02 if 65816/65802 and A=$01 if 65c02/65sc02
        cmp     #2
        beq     @L9

; check for 65sc02

        ldy     $f7
        ldx     #0
        stx     $f7
        .byte   $f7,$f7         ; nop nop on 65sc02, smb7 $f7 on 65c02
        ldx     $f7
        sty     $f7
        cpx     #$00
        bne     @L9
        lda     #4              ; CPU_65SC02 constant

@L9:    ldx     #0              ; Load high byte of word
        rts

