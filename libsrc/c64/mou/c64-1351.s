;
; Driver for the 1351 proportional mouse. Parts of the code are from
; the Commodore 1351 mouse users guide.
;
; 2009-09-26, Ullrich von Bassewitz
; 2010-02-06, Greg King
;
; The driver prevents the keyboard from interfering by changing the
; keyboard's output port into an input port while the driver reads its
; controller device.  That disables a wire that is left active by the
; Kernal.  That wire is used by the STOP-key to break out of BASIC
; programs -- CC65 programs don't use that feature.  The wire is shared
; by these keys: STOP, "Q", Commodore, Space, "2", CTRL, Left-Arrow, and
; "1".  I listed them, in order, from bit 7 over to bit 0.  The
; rightmost five keys can look like joystick switches.
;
; The driver prevents the mouse/joystick from interfering by "blinding"
; the keyboard scanner while any button/switch is active.  It changes
; the input port into an output port, then stores all zero-bits in that
; port's latch.  Reading from an output port sees the bitwise-AND of the
; latch and the input signals.  Therefore, the scanner thinks that eight
; keys are being pushed at the same time.  It doesn't know what to do
; about that condition; so, it does nothing.  The driver lets the
; scanner see normally, again, when no buttons/switches are active.
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "c64.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_1351_mou

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

SCREEN_HEIGHT   = YSIZE * 8 - 1         ; (origin is zero)
SCREEN_WIDTH    = XSIZE * 8 - 1

;----------------------------------------------------------------------------
; Global variables. The bounding box values are sorted so that they can be
; written with the least effort in the SETBOX and GETBOX routines, so don't
; reorder them.

.bss

Vars:
OldPotX:        .res    1               ; Old hw counter values
OldPotY:        .res    1

XPos:           .res    2               ; Current mouse position, X
YPos:           .res    2               ; Current mouse position, Y
XMin:           .res    2               ; X1 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box
Buttons:        .res    1               ; button status bits

OldValue:       .res    1               ; Temp for MoveCheck routine
NewValue:       .res    1               ; Temp for MoveCheck routine

.rodata

; Default values for above variables
; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefVars
        .byte   0, 0                    ; OldPotX/OldPotY
        .word   SCREEN_WIDTH/2          ; XPos
        .word   SCREEN_HEIGHT/2         ; YPos
        .word   0                       ; XMin
        .word   0                       ; YMin
        .word   SCREEN_WIDTH            ; XMax
        .word   SCREEN_HEIGHT           ; YMax
        .byte   %00000000               ; Buttons
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

; Be sure the mouse cursor is invisible and at the default location. We
; need to do that here, because our mouse interrupt handler doesn't set the
; mouse position if it hasn't changed.

        sei
        jsr     CHIDE
        lda     XPos
        ldx     XPos+1
        jsr     CMOVEX
        lda     YPos
        ldx     YPos+1
        jsr     CMOVEY
        cli

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

HIDE:   sei
        jsr     CHIDE
        cli
        rts

;----------------------------------------------------------------------------
; SHOW routine. Is called to show the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is only called
; if the mouse is currently hidden and should become visible. For most drivers,
; no special action is required besides enabling the mouse cursor.
; No return code required.

SHOW:   sei
        jsr     CSHOW
        cli
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
        sei

@L1:    lda     (ptr1),y
        sta     XMin,y
        dey
        bpl     @L1

        cli
        rts

;----------------------------------------------------------------------------
; GETBOX: Return the mouse bounding box. The parameters are passed as they
; come from the C program, that is, a pointer to a mouse_box struct in a/x.

GETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        ldy     #.sizeof (MOUSE_BOX)-1
@L1:    lda     XMin,y
        sta     (ptr1),y
        dey
        bpl     @L1
        rts

;----------------------------------------------------------------------------
; MOVE: Move the mouse to a new position. The position is passed as it comes
; from the C program, that is: X on the stack and Y in a/x. The C wrapper will
; remove the parameter from the stack on return.
; No checks are done if the new position is valid (within the bounding box or
; the screen). No return code required.
;

MOVE:   sei                             ; No interrupts

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

        cli                             ; Allow interrupts
        rts

;----------------------------------------------------------------------------
; BUTTONS: Return the button mask in a/x.

BUTTONS:
        lda     Buttons
        ldx     #0
        and     #$1F
        rts

;----------------------------------------------------------------------------
; POS: Return the mouse position in the MOUSE_POS struct pointed to by ptr1.
; No return code required.

POS:    ldy     #MOUSE_POS::XCOORD      ; Structure offset

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
        cli                             ; Enable interrupts

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

        jsr     BUTTONS                 ; Will not touch ptr1
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

IRQ:    jsr     CPREP

; Record the state of the buttons.
; Avoid crosstalk between the keyboard and the mouse.

        ldy     #%00000000              ; Set ports A and B to input
        sty     CIA1_DDRB
        sty     CIA1_DDRA               ; Keyboard won't look like mouse
        lda     CIA1_PRB                ; Read Control-Port 1
        dec     CIA1_DDRA               ; Set port A back to output
        eor     #%11111111              ; Bit goes up when button goes down
        sta     Buttons
        beq     @L0                     ;(bze)
        dec     CIA1_DDRB               ; Mouse won't look like keyboard
        sty     CIA1_PRB                ; Set "all keys pushed"

@L0:    lda     SID_ADConv1             ; Get mouse X movement
        ldy     OldPotX
        jsr     MoveCheck               ; Calculate movement vector

; Skip processing if nothing has changed

        bcc     @SkipX
        sty     OldPotX

; Calculate the new X coordinate (--> a/y)

        add     XPos
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

@SkipX: lda     SID_ADConv2             ; Get mouse Y movement
        ldy     OldPotY
        jsr     MoveCheck               ; Calculate movement

; Skip processing if nothing has changed

        bcc     @SkipY
        sty     OldPotY

; Calculate the new Y coordinate (--> a/y)

        sta     OldValue
        lda     YPos
        sub     OldValue
        tay
        stx     OldValue
        lda     YPos+1
        sbc     OldValue
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

; Move the mouse pointer to the new Y pos

        tya
        jsr     CMOVEY

; Done

@SkipY: jsr     CDRAW
        clc                             ; Interrupt not "handled"
        rts

; --------------------------------------------------------------------------
;
; Move check routine, called for both coordinates.
;
; Entry:        y = old value of pot register
;               a = current value of pot register
; Exit:         y = value to use for old value
;               x/a = delta value for position
;

MoveCheck:
        sty     OldValue
        sta     NewValue
        ldx     #$00

        sub     OldValue                ; a = mod64 (new - old)
        and     #%01111111
        cmp     #%01000000              ; if (a > 0)
        bcs     @L1                     ;
        lsr     a                       ;   a /= 2;
        beq     @L2                     ;   if (a != 0)
        ldy     NewValue                ;     y = NewValue
        sec
        rts                             ;   return

@L1:    ora     #%11000000              ; else, "or" in high-order bits
        cmp     #$FF                    ; if (a != -1)
        beq     @L2
        sec
        ror     a                       ;   a /= 2
        dex                             ;   high byte = -1 (X = $FF)
        ldy     NewValue
        sec
        rts

@L2:    txa                             ; A = $00
        clc
        rts

