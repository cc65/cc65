;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         soft80_textcolor, soft80_bgcolor, soft80_bordercolor
        .export         __textcolor, __bgcolor

        .importzp       tmp1, tmp2

        .import         soft80_checkchar

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_textcolor:
        ldx     __textcolor             ; get old value
        sta     __textcolor             ; set new value

        jsr     mkcharcolor

        txa                             ; get old value
        rts

soft80_bgcolor:
        ldx     __bgcolor               ; get old value
        stx     tmp2                    ; save old value
        sta     __bgcolor               ; set new value

        jsr     mkcharcolor

        ; if the old bg color is equal to color ram of that cell, then also
        ; update the color ram to the new value.
        ; FIXME: perhaps we must also check if the non visible character is not
        ;        a space, and NOT update the color ram in that case.
        ldx     #$00
lp1:
        .repeat $3,page
        .scope
        lda     soft80_colram+(page*$100),x
        and     #$0f
        cmp     tmp2                    ; old bg color
        bne     @sk1
        lda     __bgcolor
        sta     soft80_colram+(page*$100),x
@sk1:
        .endscope
        .endrepeat

        .scope
        lda     soft80_colram+$2e8,x
        and     #$0f
        cmp     tmp2                    ; old bg color
        bne     @sk1
        lda     __bgcolor
        sta     soft80_colram+$2e8,x
@sk1:
        .endscope

        inx
        bne     lp1

        sei
        ldy     $01
        lda     #$34                    ; disable I/O
        sta     $01

        ldx     #$00
@lp2:
        .repeat $3,page
        lda     soft80_vram+(page*$100),x
        and     #$0f
        ora     tmp1                    ; new bg color (high nibble)
        sta     soft80_vram+(page*$100),x
        .endrepeat
        lda     soft80_vram+$2e8,x
        and     #$0f
        ora     tmp1                    ; new bg color (high nibble)
        sta     soft80_vram+$2e8,x

        inx
        bne     @lp2

        sty     $01                     ; enable I/O
        cli

        lda     tmp2                    ; get old value
        rts

mkcharcolor:
        lda     __bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        sta     tmp1                    ; remember new bg color (high nibble)
        ora     __textcolor
        sta     CHARCOLOR               ; text/bg combo for new chars
        rts

soft80_bordercolor:
        ldx     VIC_BORDERCOLOR         ; get old value
        sta     VIC_BORDERCOLOR         ; set new value
        txa
        rts

        .bss
__textcolor:
        .res 1
__bgcolor:
        .res 1
