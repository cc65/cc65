        .include        "gamate.inc"
        .include        "extzp.inc"

        .import         colors
        .importzp       ptr1, tmp1

        .constructor    initconio

        .macpack        longbranch

        .segment        "ONCE"
initconio:
        lda     #0
        sta     LCD_XPOS
        sta     LCD_YPOS

        lda     #LCD_MODE_INC_Y
        sta     LCD_MODE

        lda     #COLOR_BLACK
        sta     CHARCOLOR
        lda     #COLOR_WHITE
        sta     BGCOLOR
        rts

        .segment        "RODATA"

        .export         fontdata
fontdata:
        .include        "vga.inc"
