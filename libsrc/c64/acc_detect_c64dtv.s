;
; Marco van den Heuvel, 2018-04-14
;

; unsigned char detect_c64dtv (void);
;
;/* Check for the presence of the C64DTV.
; *
; * Possible return values:
; * 0x00  : C64DTV not present
; * 0x01  : C64DTV present
; */

        .export         _detect_c64dtv

        .include        "accelerator.inc"

_detect_c64dtv:
        ldy     C64DTV_Extended_Regs
        lda     #$00
        ldx     $D000

; Make sure the CPU is a 6510
        .byte   $1A                   ; NOP on 8502, INA on 65(S)C(E)02, 4510 and 65816
        bne     not_found
        lda     #$01
        sta     C64DTV_Extended_Regs

; Check if $D000 is mirrored at $D040
        cpx     $D040
        bne     found
        inc     $D000
        cpx     $D040
        bne     not_found
found:
        lda     #$01
        .byte   $2C
not_found:
        lda     #$00
        stx     $D000
        ldx     #$00
        sty     C64DTV_Extended_Regs
        rts

