;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getmaxx (void);
; /* Return the maximum x coordinate. The resolution in x direction is
;  * getmaxx() + 1
;  */

        .include        "tgi-kernel.inc"
        .export         _tgi_getmaxx
        .import         _tgi_getxres
        .import         decax1


_tgi_getmaxx:
        jsr     _tgi_getxres
        jmp     decax1



