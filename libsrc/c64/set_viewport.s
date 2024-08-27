;
; Oliver Schmidt, 16.8.2018
;
; int __fastcall__ set_viewport (uint8_t scr_hi, uint8_t chr_hi);
;

        .include        "time.inc"
        .include        "c64.inc"

        .importzp       sreg, ptr1
        .import         pushax, pusheax, ldax0sp, ldeaxidx
        .import         tosdiveax, incsp3, return0
        .import         TM, load_tenth


;----------------------------------------------------------------------------
.code

.proc   _set_viewport
        sta     tmp1     ; chr_hi
        ldy     #0
        eor     (sp), y  ; eor scr_hi with chr_hi to determine VIC bank compliance
        sec
        bne     @return  ; If the upper two bits do not coincide, then the screen and char would be in different VIC banks
        
        sty     tmp2     ; store bank bits temporarily, here      
        lsr     tmp1     ; lsr hi-byte into correct bit position for VIC_ADDR (0x08 -> 0x02, 0x10 -> 0x04 ...)
        lsr     tmp1     ; The vic bank check prevents doing this directly in an effective manner, beforehand

        lda     (sp), y  ; scr_hi
        asl              ; rol hi-byte into correct bit position for VIC_ADDR (0x04 -> 0x10, 0x08 -> 0x20 ...)
        rol     tmp2     ; rol hi-bits into bank bits
        asl
        rol     tmp2     ; rol's 0 into carry
        ora     tmp1     ; combine scr and char bits to the final register value
        sta     VIC_ADDR

        lda     CIA2_pra
        ora     #$03     ; prepare for eor
        eor     tmp2     ; EOR bank bits, because in the register the bits are reversed
        sta     CIA2_pra
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
