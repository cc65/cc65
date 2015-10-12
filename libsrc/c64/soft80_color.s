;
; Groepaz/Hitmen, 12.10.2015
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
;

        .export         soft80_textcolor, soft80_bgcolor
        .import         soft80_internal_textcolor, soft80_internal_bgcolor

        .importzp       tmp1, tmp2

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_textcolor:
        ldx     soft80_internal_textcolor             ; get old value
        sta     soft80_internal_textcolor             ; set new value

        jsr     mkcharcolor

        txa                             ; get old value
        rts

soft80_bgcolor:
        ldx     soft80_internal_bgcolor               ; get old value
        stx     tmp2                    ; save old value
        sta     soft80_internal_bgcolor               ; set new value

        jsr     mkcharcolor

.if SOFT80COLORVOODOO = 1
        ; if the old bg color is equal to color ram of that cell, then also
        ; update the color ram to the new value.
        ; FIXME: perhaps we must also check if the non visible character is not
        ;        a space, and NOT update the color ram in that case.
        ldx     #$00
lp1:
        .repeat $4,page
        .scope
        lda     soft80_colram+(page*250),x
        and     #$0f
        cmp     tmp2                    ; old bg color
        bne     @sk1
        lda     soft80_internal_bgcolor               ; new bg color
        sta     soft80_colram+(page*250),x
@sk1:
        .endscope
        .endrepeat

        inx
        bne     lp1
.endif

        sei
        ldy     $01
        lda     #$34                    ; disable I/O
        sta     $01

        ; if the old bg color is equal to text color in this cell, then also
        ; update the text color to the new value.
        ; FIXME: perhaps we need to check for space, see note above
        ldx     #$00
lp2:
        .repeat $4,page
        .scope
        lda     soft80_vram+(page*250),x
        and     #$0f
        cmp     tmp2                    ; old bg color
        bne     @sk2
        lda     soft80_internal_bgcolor               ; new bg color
@sk2:
        ora     tmp1                    ; new bg color (high nibble)
        sta     soft80_vram+(page*250),x
        .endscope
        .endrepeat

        inx
        bne     lp2

        sty     $01                     ; enable I/O
        cli

        lda     tmp2                    ; get old value
        rts

mkcharcolor:
        lda     soft80_internal_bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        sta     tmp1                    ; remember new bg color (high nibble)
        ora     soft80_internal_textcolor
        sta     CHARCOLOR               ; text/bg combo for new chars
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
