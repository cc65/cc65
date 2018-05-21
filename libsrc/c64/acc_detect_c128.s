;
; 2018-04-20, Marco van den Heuvel
; 2018-04-26, Greg King
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
        ldx     #>$0001
        lda     #<$0001

; Make sure the CPU is an 8502.
        .byte   $3A     ; NOP on 8502, DEA on 65(S)C(E)02, 4510, and 65816
        beq     detect_end

; Make sure a C128 VIC-IIe is present.
        ldy     C128_VICIIE_CLK
        cpy     #$FF
        bne     detect_end
        txa             ; return zero when not VIC-IIe
detect_end:
        rts
