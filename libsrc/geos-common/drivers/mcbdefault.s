;
; Mouse callbacks for GEOS.
;
; GEOS has a built-in mouse architecture.  Half of this file does nothing
; -- it exists merely to allow portable programs to link and run.
;
; 2.7.2001, Maciej 'YTM/Elysium' Witkowiak
; 2004-03-20, Ullrich von Bassewitz
; 2004-09-24, Greg King
;

;       .constructor    init_pointer
        .export         _mouse_def_callbacks

;       .include        "mouse-kernel.inc"
;       .include        "const.inc"
;       .include        "geossym.inc"
        .include        "jumptab.inc"

;       .macpack        generic

; The functions below must be interrupt-safe,
; because they might be called from an interrupt-handler.

; --------------------------------------------------------------------------
; Hide the mouse pointer. Always called with interrupts disabled.

hide := MouseOff

; --------------------------------------------------------------------------
; Show the mouse pointer. Always called with interrupts disabled.

show := MouseUp

; --------------------------------------------------------------------------
; Prepare to move the mouse pointer. Always called with interrupts disabled.

prep:
        ; Fall through

; --------------------------------------------------------------------------
; Draw the mouse pointer. Always called with interrupts disabled.

draw:
        ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in .XA. Always called with
; interrupts disabled.

movex:
        ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in .XA. Always called with
; interrupts disabled.

movey:
        rts

; --------------------------------------------------------------------------
; Callback structure

.rodata

_mouse_def_callbacks:
        .addr   hide
        .addr   show
        .addr   prep
        .addr   draw
        .addr   movex
        .addr   movey
