;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_setpixel (int x, int y);
; /* Plot a point in the current drawing color */


        .include        "tgi-kernel.inc"

.proc   _tgi_setpixel

        jsr     tgi_getset      ; Pop args, check range
        bcs     @L9
        jmp     tgi_setpixel    ; Call the driver
@L9:    rts

.endproc


