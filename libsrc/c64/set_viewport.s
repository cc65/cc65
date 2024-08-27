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
        lsr
        lsr
        sta tmp1    ; chr_hi
        lda #0
        sta tmp2    ; bank
        jsr popa    ; scr_hi
        asl
        rol tmp2
        asl
        rol tmp2
        ora tmp1
        sta VIC_ADDR

        lda CIA2_pra
        ora #$03
        eor tmp2
        sta CIA2_pra

        rts
.endproc
