    						.include "pcengine.inc"
            .import vce_init
            .import psg_init
            .import vdc_init

			.export initconio
		    .export _conio_init

			.constructor initconio, 24

            .macpack longbranch
initconio:
                     ;;   jsr     vdc_init
                        jsr     vce_init
                        jsr     psg_init
                        jsr     conio_init
                        jsr     set_palette

                        st0     #VDC_RCR
                        st1     #<$0088
                        st2     #>$0088
						rts

						.import colors
set_palette:

                        ; Make palette (use VGA palette?)
                     ;   stz     VCE_ADDR_LO
                     ;   stz     VCE_ADDR_HI
                     ;   clx
                     ;   cly
        vce_loop:    ;; stx     VCE_DATA_LO
                     ;; sty     VCE_DATA_HI
                     ;   inx
                     ;   cpx     #$00
                     ;   bne     vce_loop
                     ;   iny
                     ;   cpy     #$02
                     ;   bne     vce_loop


                        stz     VCE_ADDR_LO
                        stz     VCE_ADDR_HI
						
						ldx #0
@lp:
						.repeat 16
                        lda colors,x
                        sta     VCE_DATA_LO
                        lda colors+1,x
                        sta     VCE_DATA_HI
						.endrepeat

						inx
						inx
						cpx #16*2;*5
						jne @lp

                        stz     VCE_ADDR_LO
                        stz     VCE_ADDR_HI
                        stz     VCE_DATA_LO
                        stz     VCE_DATA_HI

; so it will get linked in
_conio_init:
                        rts

;----------------------------------------------------------------------------
;
;----------------------------------------------------------------------------

                        .importzp ptr1

conio_init:

                        ; Load font
                        st0     #VDC_MAWR
                        st1     #<$2000
                        st2     #>$2000

                        ; ptr to font data
                        lda 	#<font
                        sta	 	ptr1
                        lda 	#>font
                        sta	 	ptr1+1

                        st0     #VDC_VWR            ; VWR
                        ldy     #$80            ; 128 chars
        charloop:       ldx     #$08            ; 8 bytes/char
        lineloop:
						;;lda     [$00]           ; read font byte
                        lda (ptr1)
                        staio	VDC_DATA_LO       ; bitplane 0
                        stzio	VDC_DATA_HI       ; bitplane 1

                        clc                     ; increment font pointer
                        lda     ptr1
                        adc     #$01
                        sta     ptr1
                        lda     ptr1+1
                        adc     #$00
                        sta     ptr1+1
                        dex
                        bne     lineloop        ; next bitplane 0 byte
                        ldx     #$08            ; fill bitplane 2/3 with 0
        fillloop:       st1     #$00
                        st2     #$00
                        dex
                        bne     fillloop        ; next byte
                        dey
                        bne     charloop        ; next character

                        ldx #0
                        stx	BGCOLOR
                        inx
                        stx	CHARCOLOR


                        rts

                        .rodata
font:                   .include "vga.inc"
