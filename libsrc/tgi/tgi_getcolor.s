;
; Ullrich von Bassewitz, 22.06.2002
;
; unsigned char tgi_getcolor (void);
; /* Return the current drawing color */


        .include        "tgi-kernel.inc"

.proc   _tgi_getcolor

        lda     _tgi_color      ; Get the current drawing color
        ldx     #0              ; Clear high byte
        rts

.endproc
