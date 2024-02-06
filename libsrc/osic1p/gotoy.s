;
; copied from CBM implementation
; originally by:
; Ullrich von Bassewitz, 0.08.1998
;
; void gotoy (unsigned char y);
;
        .export         _gotoy
        .import         plot
        .include        "extzp.inc"

_gotoy: sta     CURS_Y          ; Set the new position
        jmp     plot            ; And activate it
