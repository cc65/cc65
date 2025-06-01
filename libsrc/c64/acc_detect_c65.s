;
; Marco van den Heuvel, 2018-04-27
;

; unsigned char detect_c65 (void);
;
;/* Check for the presence of a C65/C64DX in C64 mode.
; *
; * Possible return values:
; * 0x00  : C65/C64DX in C64 mode not present
; * 0x01  : C65/C64DX in C64 mode present
; */

        .export         _detect_c65

        .include        "accelerator.inc"

_detect_c65:
        ldy     $D000

; Make sure the CPU is not a 65816
        clc
        .byte   $E2,$01                ; NOP #$01 on 6510 and 65(S)C02, LDA $(01,S),Y on 65CE02 and 4510, SEP #$01 on 65816
        lda     #$00
        tax
        bcs     not_found              ; carry will be set on 65816

; Make sure the CPU is not a 6510
        .byte   $1A                    ; NOP on 6510, INA on 65(S)C(E)02
        beq     not_found
        txa

; Make sure the CPU is a 65CE02/4510
        .byte   $A3,$A3                ; NOP NOP on 65(S)C02, LDZ #$A3 on 65CE02 and 4510
        .byte   $6B                    ; NOP on 65(S)C02, TZA on 65CE02 and 4510
        cmp     #$A3
        bne     not_found

; Switch to VICIII mode and check if $D040 is a mirror of $D000
        ldy     #C65_VICIII_UNLOCK_1
        sty     C65_VICIII_KEY
        ldy     #C65_VICIII_UNLOCK_2
        sty     C65_VICIII_KEY
        cpy     $D040
        bne     found
        inc     $D000
        cpy     $D040
        bne     not_found

found:
        lda     #$01
not_found:
        sty     $D000
        sta     C65_VICIII_KEY
        rts
