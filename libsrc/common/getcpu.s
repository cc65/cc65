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

; check for 4510

        lda     #5              ; CPU_65CE02 constant
        .byte   $5c             ; map on 4510, aug on 65ce02 (acts like 4 byte nop)
        lda     #3              ; CPU_4510 constant
        nop
        bne     @L9

; check for 65sc02

@L6:    ldy     $f7
        ldx     #$00
        stx     $f7
        .byte   $f7,$f7         ; nop nop on 65sc02, smb7 $f7 on 65c02 and 65816
        ldx     $f7
        sty     $f7
        cpx     #$00
        bne     @L7
        lda     #4              ; CPU_65SC02 constant
        bne     @L9

; check for 65816; after 4510, because $eb there is row (rotate word)

@L7:    xba                     ; .byte $eb, put $01 in B accu
        dec     a               ; .byte $3a, A=$00 if 65C02
        xba                     ; .byte $eb, get $01 back if 65816
        inc     a               ; .byte $1a, make $01/$02
@L9:    ldx     #0              ; Load high byte of word
        rts

