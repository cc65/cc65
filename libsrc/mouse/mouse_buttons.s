;
; Ullrich von Bassewitz, 2003-12-30
;
; unsigned char mouse_buttons (void);
; /* Return a bit mask encoding the states of the mouse buttons. Use the
; ** MOUSE_BTN_XXX flags to decode a specific button.
; */
;

        .include        "mouse-kernel.inc"

        _mouse_buttons  = mouse_buttons ; Call driver directly
