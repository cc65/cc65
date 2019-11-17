;
; Default mouse callbacks for the CX16
;
; 2019-11-09, Greg King
;
; All functions in this module should be interrupt safe
; because they might be called from an interrupt handler.
;

        .export         _mouse_def_callbacks

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

; --------------------------------------------------------------------------
; Hide the mouse pointer. Always called with interrupts disabled.

.code

hide:   ldx     #$00            ; Don't change sprite's scale
        lda     #$00            ; Disable sprite
        jmp     MOUSE

; --------------------------------------------------------------------------
; Show the mouse pointer. Always called with interrupts disabled.

show:   ldx     #$00
        lda     #<-$01          ; Enable sprite
        jmp     MOUSE

; --------------------------------------------------------------------------
; Prepare to move the mouse pointer. Always called with interrupts disabled.

prep:   ; Fall through

; --------------------------------------------------------------------------
; Draw the mouse pointer. Always called with interrupts disabled.

draw:   ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in .XA .  Always called with
; interrupts disabled.

movex:                          ; Already set by drivers
        ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in .XA .  Always called with
; interrupts disabled.

movey:  rts                     ; Already set by drivers

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
