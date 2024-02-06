;
; Ullrich von Bassewitz, 0.08.1998
;
; void gotoy (unsigned char y);
;

        .export         _gotoy
        .import         plot
        .importzp       CURS_Y

_gotoy: sta     CURS_Y          ; Set the new position
        jmp     plot            ; And activate it

