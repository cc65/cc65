

        ; soft80_cgetc.s
        .import soft80_cgetc
        .export _cgetc := soft80_cgetc

        ; soft80_chline.s
        .import soft80_chlinexy
        .import soft80_chline
        .export _chlinexy := soft80_chlinexy
        .export _chline := soft80_chline

        ; soft80_color.s
        .import soft80_textcolor
        .import soft80_bgcolor
        .import soft80_bordercolor
        .export _textcolor := soft80_textcolor
        .export _bgcolor := soft80_bgcolor
        .export _bordercolor := soft80_bordercolor

        ; soft80_cputc.s
        .import soft80_cputc
        .import soft80_cputcxy
        .import soft80_cputdirect
        .import soft80_putchar
        .import soft80_newline
        .import soft80_plot
        .export _cputc := soft80_cputc
        .export _cputcxy := soft80_cputcxy
        .export cputdirect := soft80_cputdirect
        .export putchar := soft80_putchar
        .export newline := soft80_newline
        .export plot := soft80_plot

        ; soft80_cvline.s
        .import soft80_cvlinexy
        .import soft80_cvline
        .export _cvlinexy := soft80_cvlinexy
        .export _cvline := soft80_cvline

        ; soft80_kclrscr.s
        .import soft80_kclrscr
        .export _clrscr := soft80_kclrscr
        .export CLRSCR := soft80_kclrscr

        ; soft80_kplot.s
        .import soft80_kplot
        .export PLOT := soft80_kplot

        ; soft80_kscreen.s
        .import soft80_kscreen
        .export SCREEN := soft80_kscreen

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
