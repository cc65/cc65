;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
; /* Draw a circle in the current drawing color */

        .include        "tgi-kernel.inc"

        .import         incsp4

.proc   _tgi_circle

        ; For now
        jmp     incsp4

.endproc
