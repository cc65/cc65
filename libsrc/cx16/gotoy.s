;
; 2019-11-06, Greg King
;
; void gotoy (unsigned char y);
;

        .export         _gotoy

        .import         plot
        .include        "cx16.inc"

_gotoy: sta     CURS_Y          ; Set the new position
        jmp     plot            ; And activate it
