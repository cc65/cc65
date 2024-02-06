;
; Ullrich von Bassewitz, 2009-11-05
;
; void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
; /* Draw a circle in the current drawing color */

        .include        "tgi-kernel.inc"

        .import         pusha


;----------------------------------------------------------------------------
;

.code
.proc   _tgi_circle

        jsr     pusha                   ; Push as rx
        jmp     _tgi_ellipse            ; Draw an ellipse with rx=ry

.endproc
