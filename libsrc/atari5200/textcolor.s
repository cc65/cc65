;
; Christian Groessler, 02-Apr-2019
;
; unsigned char __fastcall__ textcolor (unsigned char color);
;
; "color" value is a palette index (0..3) or COLOR_xxx value (0..3)

        .export         _textcolor
        .import         conio_color


_textcolor:
        ; move bits #0 and #1 to bits #6 and #7
        and     #3
        clc
        ror     a
        ror     a
        ror     a               ; new conio_color value
        ldx     conio_color     ; get old value
        sta     conio_color     ; store new value
        txa
        ; move bits #6 and #7 to bits #0 and #1
        clc
        rol     a
        rol     a
        rol     a
        ldx     #0
        rts
