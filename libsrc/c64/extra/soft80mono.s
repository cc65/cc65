;
; Groepaz/Hitmen, 19.10.2015
;
; import/overload stubs for the monochrome soft80 implementation
;
; - optimized for size, almost 1k smaller footprint than the full color version
; - textcolor() sets one common text color for the whole screen
;
        .include "../soft80.inc"

        ; soft80mono_cgetc.s
        .import soft80mono_cgetc
        .export _cgetc := soft80mono_cgetc              ; cgetc.s

        ; soft80mono_color.s
        .import soft80mono_textcolor
        .import soft80mono_bgcolor
        .export _textcolor := soft80mono_textcolor      ; color.s
        .export _bgcolor := soft80mono_bgcolor          ; color.s

        ; soft80mono_cputc.s
        .import soft80mono_cputc
        .import soft80mono_cputcxy
        .import soft80mono_cputdirect
        .import soft80mono_putchar
        .import soft80mono_newline
        .import soft80mono_plot
        .export _cputc := soft80mono_cputc              ; cputc.s
        .export _cputcxy := soft80mono_cputcxy          ; cputc.s
        .export cputdirect := soft80mono_cputdirect     ; cputc.s
        .export putchar := soft80mono_putchar           ; cputc.s
        .export newline := soft80mono_newline           ; cputc.s
        .export plot := soft80mono_plot                 ; cputc.s

        ; soft80mono_kclrscr.s
        .import soft80mono_kclrscr
        .export _clrscr := soft80mono_kclrscr           ; clrscr.s

        ; soft80mono_kplot.s
        .import soft80mono_kplot
        .export PLOT := soft80mono_kplot                ; kplot.s

        ; soft80_kscreen.s
        .import soft80_screensize
        .export screensize := soft80_screensize         ; _scrsize.s
        ; FIXME: use _scrsize.s/remove soft80_scrsize.s
        ;.export SCREEN := soft80_screensize             ; kernal func (kernal.s)

        ; VIC sprite data for the mouse pointer
        .export         mcb_spritememory  := soft80_spriteblock
        .export         mcb_spritepointer := (soft80_vram + $03F8)

        ; Chars used by chline () and cvline ()
        .exportzp       chlinechar = CH_HLINE
        .exportzp       cvlinechar = CH_VLINE
