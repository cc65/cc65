;
; Ullrich von Bassewitz, 2004-03-23
;
; void __fastcall__ mouse_box (int minx, int miny, int maxx, int maxy);
; /* Set the bounding box for the mouse pointer movement. The mouse X and Y
;  * coordinates will never go outside the given box.
;  * NOTE: The function does *not* check if the mouse is currently inside the
;  * given margins. The proper way to use this function therefore is:
;  *
;  * 	- Hide the mouse
;  * 	- Set the bounding box
;  * 	- Place the mouse at the desired position
;  * 	- Show the mouse again.
;  *
;  * NOTE2: When setting the box to something that is larger than the actual
;  * screen, the positioning of the mouse cursor can fail. If such margins
;  * are really what you want, you have to use your own cursor routines.
;  */
;

        .import         incsp6

        .include        "mouse-kernel.inc"

.proc   _mouse_box

        jsr     mouse_box               ; Call the driver
        jmp     incsp6                  ; Cleanup the stack

.endproc



