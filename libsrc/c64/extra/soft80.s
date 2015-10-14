;
; Groepaz/Hitmen, 12.10.2015
;
; import/overload stubs for the soft80 implementation

        .include "../soft80.inc"

        ; soft80_cgetc.s
        .import soft80_cgetc
        .export _cgetc := soft80_cgetc

        ; soft80_color.s
        .import soft80_textcolor
        .import soft80_bgcolor
        .export _textcolor := soft80_textcolor
        .export _bgcolor := soft80_bgcolor

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

        ; soft80_kclrscr.s
        .import soft80_kclrscr
        .export _clrscr := soft80_kclrscr
        .export CLRSCR := soft80_kclrscr

        ; soft80_kplot.s
        .import soft80_kplot
        .export PLOT := soft80_kplot

        ; soft80_kscreen.s
        .import soft80_screensize
        .export screensize := soft80_screensize

        ; VIC sprite data for the mouse pointer
        .export         mcb_spritememory  := soft80_spriteblock
        .export         mcb_spritepointer := (soft80_vram + $03F8)

        ; Chars used by chline () and cvline ()
        .exportzp       chlinechar = CH_HLINE
        .exportzp       cvlinechar = CH_VLINE
