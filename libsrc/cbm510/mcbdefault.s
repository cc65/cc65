;
; Default mouse callbacks for the CBM510 (P128)
;
; 2004-03-20, Ullrich von Bassewitz2
; 2013-06-25, Greg King
;
; All functions in this module should be interrupt-safe because they might
; be called from an interrupt handler.
;

        .constructor    initmcb
        .export         _mouse_def_callbacks
        .import         _mouse_def_pointershape
        .import         _mouse_def_pointercolor
        .import         vic:zp

        .include        "mouse-kernel.inc"
        .include        "cbm510.inc"

        .macpack        generic

; Sprite definitions. The first value can be changed to adjust the number
; of the sprite used for the mouse. All others depend on that value.
MOUSE_SPR       = 0                             ; Sprite used for the mouse
MOUSE_SPR_MEM   = $F400                         ; Memory location
MOUSE_SPR_MASK  = $01 .shl MOUSE_SPR            ; Positive mask
MOUSE_SPR_NMASK = .lobyte(.not MOUSE_SPR_MASK)  ; Negative mask
VIC_SPR_X       = (VIC_SPR0_X + 2*MOUSE_SPR)    ; Sprite X register
VIC_SPR_Y       = (VIC_SPR0_Y + 2*MOUSE_SPR)    ; Sprite Y register

; --------------------------------------------------------------------------
; Initialize the mouse sprite.

.segment        "ONCE"

initmcb:

; Copy the mouse sprite data

        ldx     #64 - 1
@L0:    lda     _mouse_def_pointershape,x
        sta     MOUSE_SPR_MEM,x
        dex
        bpl     @L0

; Set the mouse sprite pointer

        lda     #<(MOUSE_SPR_MEM / 64)
        sta     $F3F8 + MOUSE_SPR

; Set the mouse sprite color

        ldx     IndReg
        lda     #15
        sta     IndReg

        lda     _mouse_def_pointercolor
        ldy     #VIC_SPR0_COLOR + MOUSE_SPR
        sta     (vic),y

        stx     IndReg
        rts

; --------------------------------------------------------------------------
; Hide the mouse pointer. Always called with interrupts disabled.

.code

hide:
        ldy     #15
        sty     IndReg

        ldy     #VIC_SPR_ENA
        lda     (vic),y
        and     #MOUSE_SPR_NMASK
        sta     (vic),y

        ldy     ExecReg
        sty     IndReg
        rts

; --------------------------------------------------------------------------
; Show the mouse pointer. Always called with interrupts disabled.

show:
        ldy     #15
        sty     IndReg

        ldy     #VIC_SPR_ENA
        lda     (vic),y
        ora     #MOUSE_SPR_MASK
        sta     (vic),y

        ldy     ExecReg
        sty     IndReg
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
; Move the mouse pointer x position to the value in .XA. Always called with
; interrupts disabled.

movex:
        ldy     #15
        sty     IndReg

; Add the x correction; and, set the low byte. That frees .A.

        add     #<24                    ; x correction
        ldy     #VIC_SPR_X
        sta     (vic),y

; Set the high byte

        ldy     #VIC_SPR_HI_X
        txa
        adc     #>24
        bnz     @L1                     ; Branch if high byte not zero
        lda     (vic),y                 ; Get high x bits of all sprites
        and     #MOUSE_SPR_NMASK        ; Clear high bit for sprite
        sta     (vic),y

@L0:    ldy     ExecReg
        sty     IndReg
        rts

@L1:    lda     (vic),y                 ; Get high x bits of all sprites
        ora     #MOUSE_SPR_MASK         ; Set high bit for sprite
        sta     (vic),y
        bnz     @L0                     ; Branch always

; --------------------------------------------------------------------------
; Move the mouse pointer y position to the value in .XA. Always called with
; interrupts disabled.

movey:
        ldy     #15
        sty     IndReg

        add     #50                     ; y correction (first visible line)
        ldy     #VIC_SPR_Y
        sta     (vic),y                 ; Set y position

        ldy     ExecReg
        sty     IndReg
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
