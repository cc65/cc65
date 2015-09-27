;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         soft80_textcolor, soft80_bgcolor, soft80_bordercolor
        .export         __textcolor,__bgcolor   ; CHECK/FIX

        .importzp       tmp1,tmp2

        .import         soft80_checkchar

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_textcolor:

        ldx     __textcolor             ; get old value
        sta     __textcolor             ; set new value

        lda     __bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        ora     __textcolor
        sta     CHARCOLOR               ; text/bg combo for new chars

        txa                             ; get old value
        rts


soft80_bgcolor:
        ldx     __bgcolor               ; get old value
        sta     __bgcolor               ; set new value
        asl     a
        asl     a
        asl     a
        asl     a
        sta     tmp2                    ; shifted new value
        ora     __textcolor
        sta     CHARCOLOR               ; text/bg combo for new chars
        txa
        pha                             ; save old value
        sta     tmp1

        sei
        lda     $01
        pha

        lda     #$34
        sta     $01

        ldx     #$00

lp2:
        .repeat $4,page

        .scope
        lda     soft80_vram+(page*$100),x
        and     #$0f
        ;cmp     tmp1                    ; old bg color
        ;bne     as
        ; is old bg color
        ; is space
        ;lda __bgcolor
as:
        ora     tmp2                    ; new bg color
        sta     soft80_vram+(page*$100),x
        .endscope

        .endrepeat

        inx
        bne     lp2

        pla
        sta     $01
        cli

        pla                             ; get old value
        rts


soft80_bordercolor:
        ldx     VIC_BORDERCOLOR         ; get old value
        sta     VIC_BORDERCOLOR         ; set new value
        txa
        rts

        ; FIXME: shouldnt they be in zeropage?
        .bss
__textcolor:
        .res 1
__bgcolor:
        .res 1
