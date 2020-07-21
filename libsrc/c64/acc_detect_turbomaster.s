;
; Marco van den Heuvel, 2018-04-30
;

; unsigned char detect_turbomaster (void);
;
;/* Check for the presence of a Turbo Master cartridge.
; *
; * Possible return values:
; * 0x00  : TurboMaster cartridge not present
; * 0x01  : TurboMaster cartridge present
; */

        .export         _detect_turbomaster

        .include        "accelerator.inc"

_detect_turbomaster:
        lda     #$00
        tax

; Make sure the current CPU is not a 6510
        .byte   $1A                    ; NOP on 8502, INA on 65(S)C(E)02, 4510 and 65816
        beq     not_found

; Make sure the current CPU is not a 65816
        clc
        .byte   $E2,$01                ; NOP #$01 on 65(S)C02, LDA $(01,S),Y on 65CE02 and 4510, SEP #$01 on 65816
        bcs     not_found              ; carry will be set on 65816

; Make sure the current CPU is not a 65CE02/4510
        .byte   $A3,$A3                ; NOP NOP on 65(S)C02 and LDZ #$00 on 65CE02 and 4510
        .byte   $6B                    ; NOP on 65(S)C02 and TZA on 65CE02 and 4510
        cmp     #$A3
        beq     not_found

; Check for turbo master basic replacement
        ldy     TURBOMASTER_DETECT
        cpy     #$A2
        beq     found
not_found:
        txa
found:
        rts

