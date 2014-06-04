;
; Driver for a "joystick mouse".
;
; C128 version: Ullrich von Bassewitz, 2004-04-05, 2009-09-26
; Adapted to Atari: Christian Groessler, 2014-01-02
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "atari.inc"

        .macpack        generic
        .macpack        module

; ------------------------------------------------------------------------
; Header. Includes jump table

.ifdef __ATARIXL__
        module_header   _atrxjoy_mou
.else
        module_header   _atrjoy_mou
.endif

HEADER:

; Driver signature

        .byte   $6d, $6f, $75           ; "mou"
        .byte   MOUSE_API_VERSION       ; Mouse driver API version number

; Library reference

        .addr   $0000

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   HIDE
        .addr   SHOW
        .addr   SETBOX
        .addr   GETBOX
        .addr   MOVE
        .addr   BUTTONS
        .addr   POS
        .addr   INFO
        .addr   IOCTL
        .addr   IRQ

; Mouse driver flags

        .byte   MOUSE_FLAG_LATE_IRQ

; Callback table, set by the kernel before INSTALL is called

CHIDE:  jmp     $0000                   ; Hide the cursor
CSHOW:  jmp     $0000                   ; Show the cursor
CPREP:  jmp     $0000                   ; Prepare to move the cursor
CDRAW:  jmp     $0000                   ; Draw the cursor
CMOVEX: jmp     $0000                   ; Move the cursor to X coord
CMOVEY: jmp     $0000                   ; Move the cursor to Y coord


;----------------------------------------------------------------------------
; Constants

SCREEN_HEIGHT   = 191
SCREEN_WIDTH    = 319

.enum   JOY
        UP      = $01
        DOWN    = $02
        LEFT    = $04
        RIGHT   = $08
.endenum

;----------------------------------------------------------------------------
; Global variables. The bounding box values are sorted so that they can be
; written with the least effort in the SETBOX and GETBOX routines, so don't
; reorder them.

.bss

Vars:
YPos:           .res    2               ; Current mouse position, Y
XPos:           .res    2               ; Current mouse position, X
XMin:           .res    2               ; X1 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box
Buttons:        .res    1               ; Button mask


Temp:           .res    1               ; Temporary value used in the int handler

; Default values for above variables

.rodata

; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefVars
        .word   SCREEN_HEIGHT/2         ; YPos
        .word   SCREEN_WIDTH/2          ; XPos
        .word   0                       ; XMin
        .word   0                       ; YMin
        .word   SCREEN_WIDTH            ; XMax
        .word   SCREEN_HEIGHT           ; YMax
        .byte   0                       ; Buttons
.endproc

.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return an MOUSE_ERR_xx code in a/x.

INSTALL:

; Initialize variables. Just copy the default stuff over

        ldx     #.sizeof(DefVars)-1
@L1:    lda     DefVars,x
        sta     Vars,x
        dex
        bpl     @L1

; Make sure the mouse cursor is at the default location.

        lda     XPos
        ldx     XPos+1
        jsr     CMOVEX
        lda     YPos
        ldx     YPos+1
        jsr     CMOVEY

; Done, return zero (= MOUSE_ERR_OK)

        ldx     #$00
        txa
        rts

;----------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).

UNINSTALL       = HIDE                  ; Hide cursor on exit

;----------------------------------------------------------------------------
; HIDE routine. Is called to hide the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is only called
; if the mouse is currently visible and should get hidden. For most drivers,
; no special action is required besides hiding the mouse cursor.
; No return code required.

HIDE:   php
        sei
        jsr     CHIDE
        plp
        rts

;----------------------------------------------------------------------------
; SHOW routine. Is called to show the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is only called
; if the mouse is currently hidden and should become visible. For most drivers,
; no special action is required besides enabling the mouse cursor.
; No return code required.

SHOW:   php
        sei
        jsr     CSHOW
        plp
        rts

;----------------------------------------------------------------------------
; SETBOX: Set the mouse bounding box. The parameters are passed as they come
; from the C program, that is, a pointer to a mouse_box struct in a/x.
; No checks are done if the mouse is currently inside the box, this is the job
; of the caller. It is not necessary to validate the parameters, trust the
; caller and save some code here. No return code required.

SETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        ldy     #.sizeof (MOUSE_BOX)-1
        php
        sei

@L1:    lda     (ptr1),y
        sta     XMin,y
        dey
        bpl     @L1

        plp
        rts

;----------------------------------------------------------------------------
; GETBOX: Return the mouse bounding box. The parameters are passed as they
; come from the C program, that is, a pointer to a mouse_box struct in a/x.

GETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        ldy     #.sizeof (MOUSE_BOX)-1
        php
        sei

@L1:    lda     XMin,y
        sta     (ptr1),y
        dey
        bpl     @L1

        plp
        rts

;----------------------------------------------------------------------------
; MOVE: Move the mouse to a new position. The position is passed as it comes
; from the C program, that is: X on the stack and Y in a/x. The C wrapper will
; remove the parameter from the stack on return.
; No checks are done if the new position is valid (within the bounding box or
; the screen). No return code required.
;

MOVE:   php
        sei                             ; No interrupts

        pha
        txa
        pha
        jsr     CPREP
        pla
        tax
        pla

        sta     YPos
        stx     YPos+1                  ; New Y position
        jsr     CMOVEY                  ; Set it

        ldy     #$01
        lda     (sp),y
        sta     XPos+1
        tax
        dey
        lda     (sp),y
        sta     XPos                    ; New X position
        jsr     CMOVEX                  ; Move the cursor

        jsr     CDRAW

        plp                             ; Restore interrupt flag
        rts

;----------------------------------------------------------------------------
; BUTTONS: Return the button mask in a/x.

BUTTONS:
        lda     Buttons
        ldx     #$00
        rts

;----------------------------------------------------------------------------
; POS: Return the mouse position in the MOUSE_POS struct pointed to by ptr1.
; No return code required.

POS:    ldy     #MOUSE_POS::XCOORD      ; Structure offset

        php
        sei                             ; Disable interrupts
        lda     XPos                    ; Transfer the position
        sta     (ptr1),y
        lda     XPos+1
        iny
        sta     (ptr1),y
        lda     YPos
        iny
        sta     (ptr1),y
        lda     YPos+1
        plp                             ; Restore interrupt flag

        iny
        sta     (ptr1),y                ; Store last byte

        rts                             ; Done

;----------------------------------------------------------------------------
; INFO: Returns mouse position and current button mask in the MOUSE_INFO
; struct pointed to by ptr1. No return code required.
;
; We're cheating here to keep the code smaller: The first fields of the
; mouse_info struct are identical to the mouse_pos struct, so we will just
; call _mouse_pos to initialize the struct pointer and fill the position
; fields.

INFO:   jsr     POS

; Fill in the button state

        lda     Buttons
        ldy     #MOUSE_INFO::BUTTONS
        sta     (ptr1),y

        rts

;----------------------------------------------------------------------------
; IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an error code in a/x.
;

IOCTL:  lda     #<MOUSE_ERR_INV_IOCTL     ; We don't support ioclts for now
        ldx     #>MOUSE_ERR_INV_IOCTL
        rts

;----------------------------------------------------------------------------
; IRQ: Irq handler entry point. Called as a subroutine but in IRQ context
; (so be careful). The routine MUST return carry set if the interrupt has been
; 'handled' - which means that the interrupt source is gone. Otherwise it
; MUST return carry clear.
;

IRQ:

; Check for a pressed button and place the result into Buttons

        ldx     #0
        lda     TRIG0                   ; joystick #0 trigger
        bne     @L0                     ; not pressed
        ldx     #MOUSE_BTN_LEFT
@L0:    stx     Buttons

        lda     PORTA                   ; get joystick direction bits
        and     #15                     ; clear joystick #1 bits
        eor     #15
        sta     Temp

        jsr     CPREP

; Check left/right

        lda     Temp                    ; Read joystick #0
        and     #(JOY::LEFT | JOY::RIGHT)
        beq     @SkipX                  ;

; We will cheat here and rely on the fact that either the left, OR the right
; bit can be active

        and     #JOY::RIGHT             ; Check RIGHT bit
        bne     @Right
        lda     #$FF
        tax
        bne     @AddX                   ; Branch always
@Right: lda     #$01
        ldx     #$00

; Calculate the new X coordinate (--> a/y)

@AddX:  add     XPos
        tay                             ; Remember low byte
        txa
        adc     XPos+1
        tax

; Limit the X coordinate to the bounding box

        cpy     XMin
        sbc     XMin+1
        bpl     @L1
        ldy     XMin
        ldx     XMin+1
        jmp     @L2
@L1:    txa

        cpy     XMax
        sbc     XMax+1
        bmi     @L2
        ldy     XMax
        ldx     XMax+1
@L2:    sty     XPos
        stx     XPos+1

; Move the mouse pointer to the new X pos

        tya
        jsr     CMOVEX

; Calculate the Y movement vector

@SkipX: lda     Temp                    ; Read joystick #0
        and     #(JOY::UP | JOY::DOWN)  ; Check up/down
        beq     @SkipY                  ;

; We will cheat here and rely on the fact that either the up, OR the down
; bit can be active

        lsr     a
        bcc     @Down
        lda     #$FF
        tax
        bne     @AddY
@Down:  lda     #$01
        ldx     #$00

; Calculate the new Y coordinate (--> a/y)

@AddY:  add     YPos
        tay                             ; Remember low byte
        txa
        adc     YPos+1
        tax

; Limit the Y coordinate to the bounding box

        cpy     YMin
        sbc     YMin+1
        bpl     @L3
        ldy     YMin
        ldx     YMin+1
        jmp     @L4
@L3:    txa

        cpy     YMax
        sbc     YMax+1
        bmi     @L4
        ldy     YMax
        ldx     YMax+1
@L4:    sty     YPos
        stx     YPos+1

; Move the mouse pointer to the new X pos

        tya
        jsr     CMOVEY

; Done

@SkipY: jsr     CDRAW
        clc                             ; Interrupt not "handled"
        rts

