;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getmaxx (void);
; /* Return the maximum x coordinate. The resolution in x direction is
;  * getmaxx() + 1
;  */

        .include        "tgi-kernel.inc"

        .import         decax1


.proc   _tgi_getmaxx

        jsr     _tgi_getxres
        jmp     decax1

.endproc


