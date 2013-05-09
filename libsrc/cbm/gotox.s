;
; Ullrich von Bassewitz, 07.08.1998
;
; void gotox (unsigned char x);
;

        .export         _gotox
        .import         plot           
        .importzp       CURS_X

_gotox: sta     CURS_X          ; Set new position
        jmp     plot            ; And activate it



