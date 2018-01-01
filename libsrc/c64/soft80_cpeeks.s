;
; 2017-12-27, groepaz
;
; void cpeeks (char* s, unsigned length);
;
        .export         soft80_cpeeks
        .import         soft80_cpeekc, soft80_kplot, popax

        .importzp       ptr1, ptr2

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr2            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr2+1

        jsr     popax
        sta     ptr1
        stx     ptr1+1

        ; save current cursor position
        lda     CURS_X
        pha
        lda     CURS_Y
        pha

        ; get the string
@lp:
        jsr     soft80_cpeekc
        ldy     #0
        sta     (ptr1),y

        ; advance cursor position
        ldy     CURS_X
        ldx     CURS_Y
        iny
        cpy     #charsperline
        bne     @sk2
        ldy     #0
        inx
@sk2:
        sty     CURS_X
        stx     CURS_Y
        clc
        jsr     soft80_kplot

        inc     ptr1
        bne     @sk
        inc     ptr1+1
@sk:
        inc     ptr2
        bne     @lp
        inc     ptr2+1
        bne     @lp

        ; terminate the string
        lda     #0
        ldy     #0
        sta     (ptr1),y

        ; restore the cursor position
        pla
        tax     ; CURS_Y
        pla
        tay     ; CURS_X
        clc
        jmp     soft80_kplot
