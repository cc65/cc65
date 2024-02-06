;
; Ullrich von Bassewitz, 2009-09-26
;
; void __fastcall__ mouse_setbox (const struct mouse_box* box);
; /* Set the bounding box for the mouse pointer movement. The mouse X and Y
; ** coordinates will never go outside the given box.
; ** NOTE: The function does *not* check if the mouse is currently inside the
; ** given margins. The proper way to use this function therefore is:
; **
; **    - Hide the mouse
; **    - Set the bounding box
; **    - Place the mouse at the desired position
; **    - Show the mouse again.
; **
; ** NOTE2: When setting the box to something that is larger than the actual
; ** screen, the positioning of the mouse cursor can fail. If such margins
; ** are really what you want, you have to use your own cursor routines.
; */
;
;

        .include        "mouse-kernel.inc"

        _mouse_setbox   := mouse_setbox         ; Call driver directly


