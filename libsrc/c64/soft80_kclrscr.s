;
; Groepaz/Hitmen, 12.10.2015
;
; lowlevel kclrscr for soft80 implementation
;

        .export         soft80_kclrscr
        .import         soft80_kplot
        .import         soft80_internal_bgcolor, soft80_internal_cellcolor
        .importzp       ptr1

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kclrscr:

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

.if SOFT80COLORVOODOO = 1
        lda     soft80_internal_bgcolor
        jsr     clear                           ; clear color ram
.endif

        sei
        ldy     $01
        lda     #$34                            ; enable RAM under I/O
        sta     $01

        lda     soft80_internal_cellcolor
        and     #$f0
        ora     soft80_internal_bgcolor
        jsr     clear                           ; clear vram

        sty     $01
        cli

        ldx     #0
        ldy     #0
        clc
        jmp     soft80_kplot

        ; clear loop for colram and vram
clear:
        ;ldx     #$00
@lp1:
        sta     soft80_colram,x
        sta     soft80_colram+$100,x
        sta     soft80_colram+$200,x
        sta     soft80_colram+$2e8,x
        inx
        bne     @lp1
        rts


