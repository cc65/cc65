;
; Default mouse callbacks for the C64
;
; Ullrich von Bassewitz, 2004-03-20
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .export         _mouse_def_callbacks

        .include        "mouse-kernel.inc"
        .include        "c128.inc"

        .macpack        generic

; Sprite definitions. The first value can be changed to adjust the number
; of the sprite used for the mouse.
MOUSE_SPR       = 0                             ; Sprite used for the mouse
MOUSE_SPR_MASK  = $01 .shl MOUSE_SPR            ; Positive mask
MOUSE_SPR_NMASK = .lobyte(.not MOUSE_SPR_MASK)  ; Negative mask
VIC_SPR_X       = (VIC_SPR0_X + 2*MOUSE_SPR)    ; Sprite X register
VIC_SPR_Y       = (VIC_SPR0_Y + 2*MOUSE_SPR)    ; Sprite Y register

.code

; --------------------------------------------------------------------------
; Hide the mouse pointer. Always called with interrupts disabled.

.proc   hide

        lda     #MOUSE_SPR_NMASK
        and     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        rts

.endproc

; --------------------------------------------------------------------------
; Show the mouse pointer. Always called with interrupts disabled.

.proc   show

        lda     #MOUSE_SPR_MASK
        ora     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        rts

.endproc

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in a/x. Always called with
; interrupts disabled.

.proc   movex

; Add the X correction and set the low byte. This frees A.

        add     #24                     ; X correction
        sta     VIC_SPR_X

; Set the high byte

        txa
        adc     #0
        bne     @L1                     ; Branch if high byte not zero
   	lda	VIC_SPR_HI_X   		; Get high X bits of all sprites
   	and	#MOUSE_SPR_NMASK        ; Clear high bit for sprite
        sta     VIC_SPR_HI_X
        rts

@L1:    lda	VIC_SPR_HI_X   		; Get high X bits of all sprites
       	ora     #MOUSE_SPR_MASK         ; Set high bit for sprite
        sta     VIC_SPR_HI_X
        rts

.endproc

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in a/x. Always called with
; interrupts disabled.

.proc   movey

        clc
        ldx     PALFLAG
        bne     @L1
        adc     #50                     ; FIXME: Should be NTSC, is PAL value
       	sta    	VIC_SPR_Y               ; Set Y position
        rts

@L1:    adc     #50                     ; Add PAL correction
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


