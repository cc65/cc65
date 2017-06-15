;
; Groepaz/Hitmen, 19.10.2015
;
; lowlevel kclrscr for the monochrome soft80 implementation
;

        .export         soft80mono_kclrscr
        .import         soft80mono_kplot
        .import         soft80mono_internal_bgcolor, soft80mono_internal_cellcolor
        .importzp       ptr1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80mono_kclrscr:

        lda     #<soft80_bitmap
        sta     ptr1
        lda     #>soft80_bitmap
        sta     ptr1+1

        lda     #$ff

        ldx     #$1f
@lp2:
        ldy     #0
@lp1:
        sta     (ptr1),y
        iny
        bne     @lp1
        inc     ptr1+1
        dex
        bne     @lp2

        ;ldx     #$00
@lp3:
        sta     soft80_bitmap+$1e40,x
        inx
        bne     @lp3

        sei
        ldy     $01
        lda     #$34                            ; enable RAM under I/O
        sta     $01

        lda     soft80mono_internal_cellcolor
        ; clear loop for vram
        ;ldx     #$00
@lp4:
        sta     soft80_vram,x
        sta     soft80_vram+$100,x
        sta     soft80_vram+$200,x
        sta     soft80_vram+$2e8,x
        inx
        bne     @lp4

        sty     $01
        cli

        ldx     #0
        ldy     #0
        clc
        jmp     soft80mono_kplot
