;
; Default mouse callbacks for the C128
;
; Ullrich von Bassewitz, 2004-03-20
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .constructor    initmcb
        .export         _mouse_def_callbacks
        .import         _mouse_def_pointershape
        .import         _mouse_def_pointercolor

        .include        "mouse-kernel.inc"
        .include        "c128.inc"

        .macpack        generic

; Sprite definitions. The first value can be changed to adjust the number
; of the sprite used for the mouse. All others depend on this value.
MOUSE_SPR       = 0                             ; Sprite used for the mouse
MOUSE_SPR_MEM   = $0E00                         ; Memory location
MOUSE_SPR_MASK  = $01 .shl MOUSE_SPR            ; Positive mask
MOUSE_SPR_NMASK = .lobyte(.not MOUSE_SPR_MASK)  ; Negative mask
VIC_SPR_X       = (VIC_SPR0_X + 2*MOUSE_SPR)    ; Sprite X register
VIC_SPR_Y       = (VIC_SPR0_Y + 2*MOUSE_SPR)    ; Sprite Y register

; --------------------------------------------------------------------------
; Initialize the mouse sprite.

.segment        "INIT"

initmcb:

; Copy the mouse sprite data

        ldx     #64 - 1
@L0:    lda     _mouse_def_pointershape,x
        sta     MOUSE_SPR_MEM,x
        dex
        bpl     @L0

; Set the mouse sprite pointer

        lda     #<(MOUSE_SPR_MEM / 64)
        sta     $07F8 + MOUSE_SPR

; Set the mouse sprite color

        lda     _mouse_def_pointercolor
        sta     VIC_SPR0_COLOR + MOUSE_SPR
        rts

; --------------------------------------------------------------------------
; Hide the mouse pointer. Always called with interrupts disabled.

.code

hide:
        lda     #MOUSE_SPR_NMASK
        and     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        rts

; --------------------------------------------------------------------------
; Show the mouse pointer. Always called with interrupts disabled.

show:
        lda     #MOUSE_SPR_MASK
        ora     VIC_SPR_ENA
        sta     VIC_SPR_ENA
        ; Fall through

; --------------------------------------------------------------------------
; Prepare to move the mouse pointer. Always called with interrupts disabled.

prep:
        ; Fall through

; --------------------------------------------------------------------------
; Draw the mouse pointer. Always called with interrupts disabled.

draw:
        rts

; --------------------------------------------------------------------------
; Move the mouse pointer X position to the value in a/x. Always called with
; interrupts disabled.

movex:

; Add the X correction and set the low byte. This frees A.

        add     #24                     ; X correction
        sta     VIC_SPR_X

; Set the high byte

        txa
        adc     #0
        bne     @L1                     ; Branch if high byte not zero
        lda     VIC_SPR_HI_X            ; Get high X bits of all sprites
        and     #MOUSE_SPR_NMASK        ; Clear high bit for sprite
        sta     VIC_SPR_HI_X
        rts

@L1:    lda     VIC_SPR_HI_X            ; Get high X bits of all sprites
        ora     #MOUSE_SPR_MASK         ; Set high bit for sprite
        sta     VIC_SPR_HI_X
        rts

; --------------------------------------------------------------------------
; Move the mouse pointer Y position to the value in a/x. Always called with
; interrupts disabled.

movey:
        clc
        ldx     PALFLAG
        bne     @L2
        adc     #50                     ; FIXME: Should be NTSC, is PAL value
        sta     VIC_SPR_Y               ; Set Y position
        rts

@L2:    adc     #50                     ; Add PAL correction
        sta     VIC_SPR_Y               ; Set Y position
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
