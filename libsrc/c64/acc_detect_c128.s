;
; Marco van den Heuvel, 2018-04-20
;

; unsigned char detect_c128 (void);
;
;/* Check for the presence of a C128 in C64 mode.
; *
; * Possible return values:
; * 0x00  : C128 in C64 mode not present
; * 0x01  : C128 in C64 mode present
; */

        .export         _detect_c128

        .include        "accelerator.inc"

_detect_c128:
        lda     #$00
        tax

; Make sure the CPU is a 8502
        .byte   $1A                   ; NOP on 8502, INA on 65(S)C(E)02, 4510 and 65816
        bne     not_found

; Make sure a C128 VICIIe is present
        ldy     C128_VICIIE_CLK
        cpy     #$FF
        beq     not_found
found:
        lda     #$01
not_found:
        rts
