;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtextxy (int x, int y, const char* s);
; /* Output text at the given position. */


        .include        "tgi-kernel.inc"

        .import         addysp1
        .importzp       sp

.proc   _tgi_outtextxy

; Get the X/Y parameters and store them into curx/cury. This enables us
; to use tgi_outtext for the actual output

        pha                     ;
        ldy     #0
        lda     (sp),y
        sta     _tgi_cury
        iny
        lda     (sp),y
        sta     _tgi_cury+1
        iny
        lda     (sp),y
        sta     _tgi_curx
        iny
        lda     (sp),y
        sta     _tgi_curx+1
        pla
        jsr     addysp1         ; Drop arguments from stack

        jmp     _tgi_outtext

.endproc

