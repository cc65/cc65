;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
; /* Draw a line in the current drawing color */


        .include        "tgi-kernel.inc"

        .import         popax

.proc   _tgi_line

        jsr     tgi_linepop     ; Pop/store Y2/X2
        jsr     popax
        jsr     tgi_popxy       ; Pop/store X1/Y1 into ptr1/ptr2
        jmp     tgi_line        ; Call the driver

.endproc


