;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_lineto (int x2, int y2);
; /* Draw a line in the current drawing color from the graphics cursor to the
; ** new end point.
; */

        .include        "tgi-kernel.inc"

        .import         popax

.proc   _tgi_lineto

        pha
        ldy     #3              ; Copy curx/cury to tgi_clip_x1/tgi_clip_y1
@L1:    lda     _tgi_curx,y
        sta     tgi_clip_x1,y
        dey
        bpl     @L1  
        pla
        jsr     tgi_linepop     ; Pop x2/y2
        jmp     tgi_clippedline ; Call the line clipper

.endproc

