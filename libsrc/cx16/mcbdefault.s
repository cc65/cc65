;
; Default mouse callbacks for the CX16
;
; 2019-12-25, Greg King
;
; All functions in this module should be interrupt-safe
; because they might be called from an interrupt handler.
;

        .export         _mouse_def_callbacks

        .include        "cx16.inc"


msprite:
        stz     VERA::CTRL      ; set address for VERA's data port zero
        lda     #<(VERA::SPRITE::ATTRIB::Z_FLIP + 0 * 8)
        ldx     #>(VERA::SPRITE::ATTRIB::Z_FLIP + 0 * 8)
        ldy     #^(VERA::SPRITE::ATTRIB::Z_FLIP + 0 * 8) | VERA::INC0
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        sty     VERA::ADDR+2
        rts

; --------------------------------------------------------------------------
; Hide the mouse pointer.

hide:   jsr     msprite
        lda     VERA::DATA0
        and     #<~VERA::SPRITE::DEPTH::LAYER1
        sta     VERA::DATA0
        rts

; --------------------------------------------------------------------------
; Show the mouse pointer.

show:   jsr     msprite
        lda     VERA::DATA0
        ora     #VERA::SPRITE::DEPTH::LAYER1
        sta     VERA::DATA0
        rts

; --------------------------------------------------------------------------
; Prepare to move the mouse pointer.

prep:   ; Fall through

; --------------------------------------------------------------------------
; Draw the mouse pointer.

draw:   ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in .XA .

movex:                          ; Already set by drivers
        ; Fall through

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in .XA .

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
