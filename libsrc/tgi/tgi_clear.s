;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_clear (void);
; /* Clear the screen */

        .include        "tgi-kernel.inc"

        .export         _tgi_clear

_tgi_clear      = tgi_clear               ; Call the driver






