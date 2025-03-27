;
; Stefan 'MonteCarlos' Andree, 27.08.2024
;
; int __fastcall__ set_viewport (uint8_t scr_hi, uint8_t chr_hi);
;

        .include        "c64.inc"

        .importzp       sp, tmp1, tmp2
        .import         incsp1
        .export         _set_viewport

;----------------------------------------------------------------------------
.code

.proc   _set_viewport
        ; on input, a contains chr_hi and stack contains scr_hi

        sta     tmp1     ; save chr_hi for later
        ldy     #0       ; scr_hi stack offset
        eor     (sp), y  ; eor scr_hi with chr_hi to determine VIC bank compliance
        and     #$c0
        sec
        bne     @return  ; If the upper two bits do not coincide, then the screen and char would be in different VIC banks

        sty     tmp2     ; store bank bits temporarily, here
        lsr     tmp1     ; lsr hi-byte into correct bit position for VIC_VIDEO_ADR (0x08 -> 0x02, 0x10 -> 0x04 ...)
        lsr     tmp1     ; The vic bank check prevents doing this directly in an effective manner, beforehand

        lda     (sp), y  ; scr_hi
        asl              ; rol hi-byte into correct bit position for VIC_VIDEO_ADR (0x04 -> 0x10, 0x08 -> 0x20 ...)
        rol     tmp2     ; rol hi-bits into bank bits
        asl
        rol     tmp2     ; rol's 0 into carry which results in returning EXIT_SUCCESS, later
        ora     tmp1     ; combine scr and char bits to the final register value
        sta     VIC_VIDEO_ADR

        lda     CIA2_PRA
        ora     #$03     ; prepare for eor
        eor     tmp2     ; EOR bank bits, because in the register the bits are reversed
        sta     CIA2_PRA
    @return:
        jsr     incsp1   ; does not influence carry
        ; return EXIT_SUCCESS, if clc
        ; return EXIT_FAILURE, if sec
        lda     #0
        tax
        bcc     :+
        adc     #0
    :
        rts
.endproc
