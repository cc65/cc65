;
; Marco van den Heuvel, 2018-04-23
;

; unsigned char detect_c128 (void);
;
;/* Check if the C128 8502 CPU is the current CPU.
; *
; * Possible return values:
; * 0x00  : C128 8502 is not the current CPU
; * 0x01  : C128 8502 is the current CPU
; */

        .export         _detect_c128

        .include        "accelerator.inc"

_detect_c128:
        ldx     #$00
        lda     #$01

; Make sure the CPU is a 8502
        .byte   $3A                   ; NOP on 8502, DEA on 65(S)C(E)02, 4510 and 65816
        rts
