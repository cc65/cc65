;
; copied from CBM implementation
; originally by:
; Ullrich von Bassewitz, 07.08.1998
;
; void gotox (unsigned char x);
;
        .export         _gotox
        .import         plot
        .include        "extzp.inc"

_gotox: sta     CURS_X          ; Set new position
        jmp     plot            ; And activate it
