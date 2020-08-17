;
; Marco van den Heuvel, 2018-05-04
;

; unsigned char detect_fastchip (void);
;
;/* Check for the presence of the Apple IIe Fast Chip slot card.
; *
; * Possible return values:
; * 0x00  : Fast Chip slot card not present
; * 0x01  : Fast Chip slot card present
; */

        .export         _detect_fastchip
        .import         _get_ostype

        .include        "accelerator.inc"

_detect_fastchip:
        jsr     _get_ostype
        ldx     #$00
        cmp     #$30                ; Apple IIe
        beq     check_slot
        cmp     #$31                ; Apple IIe enhanced
        beq     check_slot
        cmp     #$40                ; Apple IIe Option Card
        bne     not_found

check_slot:
        ldy     #$04                ; loop 4 times
        lda     #FASTCHIP_UNLOCK    ; load the unlock value
unlock_loop:
        sta     FASTCHIP_LOCK_REG   ; store in lock register
        dey
        bne     unlock_loop
        sta     FASTCHIP_ENABLE_REG ; enable the Fast Chip
        ldy     FASTCHIP_ENABLE_REG ; bit 7 will be high when enabled
        bpl     not_found
found:
        lda     #FASTCHIP_LOCK
        sta     FASTCHIP_LOCK_REG   ; lock the registers again
        lda     #$01
        .byte   $24
not_found:
        txa
        rts
