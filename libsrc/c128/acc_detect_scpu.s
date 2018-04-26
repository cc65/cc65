;
; Marco van den Heuvel, 2018-04-08
;

; unsigned char detect_scpu (void);
;
;/* Check for the presence of the SuperCPU cartridge.
; *
; * Possible return values:
; * 0x00  : SuperCPU cartridge not present
; * 0x01  : SuperCPU cartridge present
; */

        .export         _detect_scpu

        .include        "accelerator.inc"
_detect_scpu:
        ldx     #$00
        txa

; Make sure the current CPU is a 65816
        clc
        .byte   $E2,$01                ; NOP #$01 on 6510 and 65(S)C02, LDA $(01,S),Y on 65CE02 and 4510, SEP #$01 on 65816
        bcc     not_found              ; carry will be set on 65816

; 65816 has been detected, make sure it's the SuperCPU cartridge

        lda     SuperCPU_Detect
        asl
        bcs     not_found
found:
        lda     #$01
not_found:
        rts
