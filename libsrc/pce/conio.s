        .constructor    initconio, 24

        .import         vce_init
        .import         psg_init
        .import         vdc_init
        .import         colors
        .importzp       ptr1, tmp1

        .include        "pce.inc"
        .include        "extzp.inc"
        .macpack        longbranch

        .segment        "ONCE"
initconio:
        jsr     vce_init
        jsr     psg_init
        jsr     conio_init
        jsr     set_palette

        st0     #VDC_CR
        st1     #<$0088
        st2     #>$0088
        rts

set_palette:
        stz     VCE_ADDR_LO
        stz     VCE_ADDR_HI

        ldx     #0
@lp:
        ldy     #16             ; size of a pallette
@lp1:
        lda     colors,x
        sta     VCE_DATA_LO
        lda     colors+1,x
        sta     VCE_DATA_HI
        dey
        bne     @lp1

        inx
        inx
        cpx     #16 * 2
        jne     @lp

; Set background to black.

        stz     VCE_ADDR_LO
        stz     VCE_ADDR_HI
        stz     VCE_DATA_LO
        stz     VCE_DATA_HI

        rts

;----------------------------------------------------------------------------
; The character tiles use only two colors from each pallette.  Color zero
; comes from pallette zero; color one is different in each pallette.  The
; color of a character is set by choosing one of the 16 pallettes.

conio_init:
        ; Load font
        st0     #VDC_MAWR
        st1     #<$2000
        st2     #>$2000

        ; pointer to font data
        lda     #<font
        sta     ptr1
        lda     #>font
        sta     ptr1+1

        st0     #VDC_VWR        ; VWR

        lda     #0
        sta     tmp1
        jsr     copy

        lda     #<font
        sta     ptr1
        lda     #>font
        sta     ptr1+1

        lda     #$FF
        sta     tmp1
        jsr     copy

        ldx     #0              ; white on black
        stx     BGCOLOR
        inx
        stx     CHARCOLOR

        rts

copy:
        ldy     #$80            ; 128 chars
charloop:
        ldx     #$08            ; 8 bytes/char
lineloop:
        lda     (ptr1)
        eor     tmp1
        sta     VDC_DATA_LO     ; bitplane 0
        stz     VDC_DATA_HI     ; bitplane 1

        clc                     ; increment font pointer
        lda     ptr1
        adc     #$01
        sta     ptr1
        lda     ptr1+1
        adc     #$00
        sta     ptr1+1
        dex
        bne     lineloop        ; next bitplane-0 byte
        ldx     #$08            ; fill bitplanes 2 and 3 with 0
fillloop:
        st1     #<$0000
        st2     #>$0000
        dex
        bne     fillloop        ; next byte
        dey
        bne     charloop        ; next character

        rts

.rodata
font:   .include        "vga.inc"
