;
; 2019-11-06, Greg King
;
; void fastcall gotox (unsigned char x);
;

        .export         _gotox

        .import         plot
        .include        "cx16.inc"

_gotox: sta     CURS_X          ; Set new position
        jmp     plot            ; And activate it
