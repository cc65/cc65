;
; Groepaz/Hitmen, 19.10.2015
;
; import/overload stubs for the monochrome conio implementation
;

        ; mono_conio.s
        .import _mono_screen
        .export _screen := _mono_screen

        ; mono_clrscr.s
        .import _mono_clrscr
        .export _clrscr := _mono_clrscr

        ; mono_cputc.s
        .import _mono_cputc
        .export _cputc := _mono_cputc

        ; mono_font.s
        .import _mono_font
        .export _font := _mono_font

        ; mono_setcursor.s
        .import mono_gotoxy
        .export gotoxy := mono_gotoxy
        .import _mono_gotoxy
        .export _gotoxy := _mono_gotoxy

