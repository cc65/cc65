;
; Marco van den Heuvel, 2018-04-25
;

; unsigned char detect_chameleon (void);
;
;/* Check for the presence of the Chameleon cartridge.
; *
; * Possible return values:
; * 0x00  : Chameleon cartridge not present
; * 0x01  : Chameleon cartridge present
; */

        .export         _detect_chameleon

        .include        "accelerator.inc"

_detect_chameleon:
        lda     #$00
        tax

; Make sure the CPU is a 6510
        .byte   $1A                   ; NOP on 6510, INA on 65(S)C(E)02, 4510 and 65816
        bne     not_found

        ldy     CHAMELEON_CFGENA
        lda     #CHAMELEON_ENABLE_REGS
        sta     CHAMELEON_CFGENA
        lda     CHAMELEON_CFGENA
        sty     CHAMELEON_CFGENA
        cmp     #$FF
        beq     not_found
found:
        lda     #$01
        .byte   $24
not_found:
        txa
        rts

