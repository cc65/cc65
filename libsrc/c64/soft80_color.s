;
; Groepaz/Hitmen, 12.10.2015
;
; high level implementation for the soft80 implementation
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
;

        .export         soft80_textcolor, soft80_bgcolor
        .import         soft80_internal_cellcolor, soft80_internal_bgcolor
        .import         soft80_internal_cursorxlsb
        .import         soft80_kplot, soft80_checkchar

        .importzp       tmp1, tmp2

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_textcolor:
        ldx     CHARCOLOR                       ; get old value
        sta     CHARCOLOR                       ; set new value

mkcharcolor:
        lda     soft80_internal_bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        sta     tmp1                            ; remember new bg color (high nibble)
        ora     CHARCOLOR
        sta     soft80_internal_cellcolor       ; text/bg combo for new chars

        txa                                     ; get old value
        rts

soft80_bgcolor:
        ldx     soft80_internal_bgcolor         ; get old value
        stx     tmp2                            ; save old value
        sta     soft80_internal_bgcolor         ; set new value

        jsr     mkcharcolor

        lda     CURS_X
        pha
        lda     CURS_Y
        pha

        ldy     #0
        ldx     #0
        clc
        jsr     soft80_kplot

        sei
        lda     $01
        pha
        ldx     #$34
        stx     $01                             ; $34

        ;ldy     #0                              ; is still 0

        lda     #24
        sta     CURS_Y
lpy:
        lda     #39
        sta     CURS_X
lpx:

.if SOFT80COLORVOODOO = 1
        ; if the old bg color is equal to color ram of that cell, then also
        ; update the color ram to the new value.

        inc     $01                             ; $35
        lda     (CRAM_PTR),y                    ; colram
        stx     $01                             ; $34

        and     #$0f
        cmp     tmp2                            ; old bg color
        bne     @sk1

        ; if the left character in the cell is not a space, then dont update
        ; the color ram
        lda     #1
        sta     soft80_internal_cursorxlsb
        jsr     soft80_checkchar
        bcc     @sk1
        lda     soft80_internal_bgcolor         ; new bg color

        inc     $01                             ; $35
        sta     (CRAM_PTR),y                    ; colram
        stx     $01                             ; $34
@sk1:
.endif
        ; if the old bg color is equal to text color in this cell, then also
        ; update the text color to the new value.

        lda     (CRAM_PTR),y                    ; vram
        and     #$0f
        cmp     tmp2                            ; old bg color
        bne     @sk2

        ; if there are non space characters in the cell, do not update the
        ; color ram
        pha
        lda     #0
        sta     soft80_internal_cursorxlsb
        jsr     soft80_checkchar
        pla
        bcc     @sk2

        pha
        inc     soft80_internal_cursorxlsb
        jsr     soft80_checkchar
        pla
        bcc     @sk2

        lda     soft80_internal_bgcolor         ; new bg color
@sk2:
        ora     tmp1                            ; new bg color (high nibble)
        sta     (CRAM_PTR),y                    ; vram

        inc     CRAM_PTR
        bne     @sk3
        inc     CRAM_PTR+1
@sk3:

        lda     SCREEN_PTR
        clc
        adc     #8
        sta     SCREEN_PTR
        bcc     @sk4
        inc     SCREEN_PTR+1
@sk4:

        dec     CURS_X
        bpl     lpx

        dec     CURS_Y
        bpl     lpy

        pla
        sta     $01                             ; enable I/O
        cli

        pla     ; CURS_Y
        tax
        pla     ; CURS_X
        tay
        clc
        jsr     soft80_kplot

        lda     tmp2                            ; get old value
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
