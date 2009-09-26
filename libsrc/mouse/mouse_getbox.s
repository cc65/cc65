;
; Ullrich von Bassewitz, 2009-09-26
;
; void __fastcall__ mouse_getbox (struct mouse_box* box);
; /* Get the current bounding box for the mouse pointer movement. */
;
;

        .include        "mouse-kernel.inc"

        _mouse_getbox    := mouse_getbox        ; Call driver directly


