;
; Ullrich von Bassewitz, 2009-11-05
;
; void __fastcall__ tgi_ellipse (int x, int y, unsigned char rx, unsigned char ry);
; /* Draw a full ellipse with center at x/y and radii rx/ry using the current
; ** drawing color.
; */
;


        .include        "tgi-kernel.inc"

        .import         pusha, push0


;----------------------------------------------------------------------------
;

.code
.proc   _tgi_ellipse

        jsr     pusha                   ; Push ry
        jsr     push0                   ; Start angle is 0
        lda     #<360
        ldx     #>360                   ; End angle is 360
        jmp     _tgi_arc

.endproc
