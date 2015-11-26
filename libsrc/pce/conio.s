        .constructor    initconio, 24

        .import         vdc_init
        .import         psg_init
        .import         colors
        .importzp       ptr1, tmp1

        .include        "pce.inc"
        .include        "extzp.inc"

        .segment        "ONCE"
initconio:
        jsr     vdc_init
        jsr     psg_init
        jsr     load_font

set_palette:
        stz     VCE_ADDR_LO
        stz     VCE_ADDR_HI

        clx
@lp:    ldy     #16             ; size of a palette

@lp1:   lda     colors,x
        sta     VCE_DATA_LO
        lda     colors+1,x
        sta     VCE_DATA_HI
        dey
        bne     @lp1

        inx
        inx
        cpx     #16 * 2         ; 16 palettes
        bne     @lp

        sty     BGCOLOR         ; white on black
        iny
        sty     CHARCOLOR

        VREG    VDC_CR, $0088   ; enable background and vertical-blank interrupt
        rts

; Load the conio font into the VDC.
load_font:
        VREG    VDC_MAWR, $2000
        st0     #VDC_VWR

        stz     tmp1            ; #%00000000
        bsr     copy            ; make normal characters

        dec     tmp1            ; #%11111111
;       bsr     copy            ; make reversed characters
;       rts                     ; (fall through)

; Point to the font data.
copy:   lda     #<font
        ldx     #>font
        sta     ptr1
        stx     ptr1+1

        ldy     #$80            ; 128 chars
charloop:
        ldx     #$08            ; 8 bytes/char
lineloop:
        lda     (ptr1)
        eor     tmp1
        sta     VDC_DATA_LO     ; bitplane 0
        st2     #>$0000         ; bitplane 1

        inc     ptr1            ; increment font pointer
        bne     @noC
        inc     ptr1+1
@noC:   dex
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
