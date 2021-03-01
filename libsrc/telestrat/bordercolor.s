;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ bordercolor (unsigned char color);
;


        .export         _bordercolor

        .include        "telestrat.inc"

_bordercolor:
        ; Nothing to do
        ; Oric can't handle his border
        rts

