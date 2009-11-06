;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
; /* Draw a line in the current drawing color */


        .include        "tgi-kernel.inc"

        .import         popax

.proc   _tgi_line

        jsr     tgi_linepop             ; Pop/store Y2/X2
        jsr     popax                   ; Y1
        sta     tgi_clip_y1
        stx     tgi_clip_y1+1
        jsr     popax                   ; X1
        sta     tgi_clip_x1
        stx     tgi_clip_x1+1
        jmp     tgi_clippedline         ; Call the line clipper

.endproc


