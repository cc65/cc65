;
; Groepaz/Hitmen, 12.10.2015
;
; import/overload stubs for the soft80 implementation

        .include "../soft80.inc"

        ; soft80_cgetc.s
        .import soft80_cgetc
        .export _cgetc := soft80_cgetc                  ; cgetc.s

        ; soft80_color.s
        .import soft80_textcolor
        .import soft80_bgcolor
        .export _textcolor := soft80_textcolor          ; color.s
        .export _bgcolor := soft80_bgcolor              ; color.s

        ; soft80_cpeekc.s
        .import soft80_cpeekc
        .export _cpeekc := soft80_cpeekc                ; cpeekc.s

        ; soft80_cpeekcolor.s
        .import soft80_cpeekcolor
        .export _cpeekcolor := soft80_cpeekcolor        ; cpeekcolor.s

        ; soft80_cpeekrevers.s
        .import soft80_cpeekrevers
        .export _cpeekrevers := soft80_cpeekrevers      ; cpeekrevers.s

        ; soft80_cpeeks.s
        .import soft80_cpeeks
        .export _cpeeks := soft80_cpeeks                ; cpeeks.s

        ; soft80_cputc.s
        .import soft80_cputc
        .import soft80_cputcxy
        .import soft80_cputdirect
        .import soft80_putchar
        .import soft80_newline
        .import soft80_plot
        .export _cputc := soft80_cputc                  ; cputc.s
        .export _cputcxy := soft80_cputcxy              ; cputc.s
        .export cputdirect := soft80_cputdirect         ; cputc.s
        .export putchar := soft80_putchar               ; cputc.s
        .export newline := soft80_newline               ; cputc.s
        .export plot := soft80_plot                     ; cputc.s

        ; soft80_kclrscr.s
        .import soft80_kclrscr
        .export _clrscr := soft80_kclrscr               ; clrscr.s

        ; soft80_kplot.s
        .import soft80_kplot
        .export PLOT := soft80_kplot                    ; kplot.s

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

        .import         return1
        .export         _doesclrscrafterexit := return1
