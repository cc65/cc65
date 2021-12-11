;
; Default mouse callbacks for the CX16
;
; 2020-01-10, Greg King
;

        .export         _mouse_def_callbacks

        .import         MOUSE_GET, SPRITE_SET_POSITION
        .include        "cx16.inc"


; --------------------------------------------------------------------------
; Hide the mouse pointer.

hide:   ldx     #%10000000
        stx     gREG::r0H
        bra     mse

; --------------------------------------------------------------------------
; Show the mouse pointer.

show:   ldx     #gREG::r0
        jsr     MOUSE_GET
mse:    lda     #$00                    ; mouse sprite
        jmp     SPRITE_SET_POSITION

; --------------------------------------------------------------------------
; Prepare to move the mouse pointer.

prep:   ; Fall through

; --------------------------------------------------------------------------
; Draw the mouse pointer.

draw:   ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in .XA .

movex:                                  ; Already done by Kernal
        ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in .XA .

movey:  rts                             ; Already done by Kernal

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
