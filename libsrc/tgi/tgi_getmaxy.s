;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getmaxy (void);
; /* Return the maximum y coordinate. The resolution in y direction is
;  * getmaxy() + 1
;  */

        .include        "tgi-kernel.inc"
        .export         _tgi_getmaxy
        .import         _tgi_getyres
        .import         decax


_tgi_getmaxy:
        jsr     _tgi_getyres
        jmp     decax1


