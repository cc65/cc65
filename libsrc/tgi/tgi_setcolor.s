;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_setcolor (unsigned char color);
; /* Set the current drawing color */


        .include        "tgi-kernel.inc"

        .export         _tgi_setcolor

_tgi_setcolor   = tgi_setcolor  ; Call the driver


