;
; Default mouse callbacks for the C64
;
; Ullrich von Bassewitz, 2004-03-20
;

        .export         _mouse_def_callbacks

        .include        "mouse-kernel.inc"
        .include        "c64.inc"


; Sprite definitions. The first value can be changed to adjust the number
; of the sprite used for the mouse.
MOUSE_SPR       = 0                             ; Sprite used for the mouse
MOUSE_SPR_MASK  = $01 .shl MOUSE_SPR            ; Positive mask
MOUSE_SPR_NMASK = .lobyte(.not MOUSE_SPR_MASK)  ; Negative mask
VIC_SPR_X       = (VIC_SPR0_X + 2*MOUSE_SPR)    ; Sprite X register
VIC_SPR_Y       = (VIC_SPR0_Y + 2*MOUSE_SPR)    ; Sprite Y register


.code

; --------------------------------------------------------------------------
; Hide the mouse pointer

.proc   hide

        lda     #MOUSE_SPR_NMASK
        sei
        and     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        cli
        rts

.endproc

; --------------------------------------------------------------------------
; Show the mouse pointer

.proc   show

        lda     #MOUSE_SPR_MASK
        sei
        ora     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        cli
        rts

.endproc

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in a/x

.proc   movex

; Set the low byte, this frees A

        sta     VIC_SPR_X

; Set the high byte

        txa                             ; Test high byte of X coord
        bne     @L1
        sei
   	lda	VIC_SPR_HI_X   		; Get high X bits of all sprites
   	and	#MOUSE_SPR_NMASK        ; Clear high bit for sprite
        sta     VIC_SPR_HI_X
        cli
        rts

@L1:    sei
   	lda	VIC_SPR_HI_X   		; Get high X bits of all sprites
   	ora     #MOUSE_SPR_NMASK        ; Set high bit for sprite
        sta     VIC_SPR_HI_X
        cli
        rts

.endproc

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in a/x

.proc   movey

       	sta    	VIC_SPR_Y               ; Set Y position
        rts

.endproc

; --------------------------------------------------------------------------
; Callback structure

.rodata

_mouse_def_callbacks:
        .addr   hide
        .addr   show
        .addr   movex
        .addr   movey



