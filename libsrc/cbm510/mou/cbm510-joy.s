;
; Driver for a "joystick mouse".
;
; 2009-09-26, Ullrich von Bassewitz
; 2014-09-10, Greg King
;

        .include        "zeropage.inc"
        .include        "../extzp.inc"

        .include        "mouse-kernel.inc"
        .include        "cbm510.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cbm510_joy_mou

HEADER:

; Driver signature

        .byte   $6d, $6f, $75           ; ASCII "mou"
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

; Callback table, set by the kernel before INSTALL is called.

CHIDE:  jmp     $0000                   ; Hide the cursor
CSHOW:  jmp     $0000                   ; Show the cursor
CPREP:  jmp     $0000                   ; Prepare to move the cursor
CDRAW:  jmp     $0000                   ; Draw the cursor
CMOVEX: jmp     $0000                   ; Move the cursor to x co-ord.
CMOVEY: jmp     $0000                   ; Move the cursor to y co-ord.


;----------------------------------------------------------------------------
; Constants

SCREEN_HEIGHT   = YSIZE * 8
SCREEN_WIDTH    = XSIZE * 8

.scope  JOY
        UP      = %00000001
        DOWN    = %00000010
        LEFT    = %00000100
        RIGHT   = %00001000
        FIRE    = %00010000
.endscope

;----------------------------------------------------------------------------
; Global variables. The bounding box values are sorted so that they can be
; written with the least effort in the SETBOX and GETBOX routines; so, don't
; re-order them.

.bss

Vars:
YPos:           .res    2               ; Current mouse position, y
XPos:           .res    2               ; Current mouse position, x
XMin:           .res    2               ; X1 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box

; Temporary value used in the interrupt handler

Temp:           .res    1

.rodata

; Default values for above variables
; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefVars
        .word   SCREEN_HEIGHT / 2       ; YPos
        .word   SCREEN_WIDTH / 2        ; XPos
        .word   0                       ; XMin
        .word   0                       ; YMin
        .word   SCREEN_WIDTH - 1        ; XMax
        .word   SCREEN_HEIGHT - 1       ; YMax
.endproc

.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return a MOUSE_ERR_xx code in .XA.

INSTALL:

; Initiate variables; just copy the default stuff over.

        ldx     #.sizeof(DefVars) - 1
@L1:    lda     DefVars,x
        sta     Vars,x
        dex
        bpl     @L1

; Be sure the mouse cursor is invisible and at the default location. We
; need to do that here because our mouse interrupt handler doesn't set the
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

; Done, return zero.

        ldx     #>MOUSE_ERR_OK
        txa
        rts

;----------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory, on return).

UNINSTALL       := HIDE                 ; Hide cursor, on exit

;----------------------------------------------------------------------------
; HIDE routine. Is called to hide the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide; and, the driver entry point is called only
; if the mouse currently is visible and should get hidden. For most drivers,
; no special action is required besides hiding the mouse cursor.
; No return code required.

HIDE:   sei
        jsr     CHIDE
        cli
        rts

;----------------------------------------------------------------------------
; SHOW routine. Is called to show the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide; and, the driver entry point is called only
; if the mouse currently is hidden and should become visible. For most drivers,
; no special action is required besides enabling the mouse cursor.
; No return code required.

SHOW:   sei
        jsr     CSHOW
        cli
        rts

;----------------------------------------------------------------------------
; SETBOX: Set the mouse bounding box. The parameters are passed as they come
; from the C program, that is, a pointer to a mouse_box struct in .XA.
; No checks are done if the mouse currently is inside the box; that is the job
; of the caller. It is not necessary to validate the parameters; trust the
; caller, and save some code here. No return code required.

SETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        ldy     #.sizeof (MOUSE_BOX) - 1
        sei

@L1:    lda     (ptr1),y
        sta     XMin,y
        dey
        bpl     @L1

        cli
        rts

;----------------------------------------------------------------------------
; GETBOX: Return the mouse bounding box. The parameters are passed as they
; come from the C program, that is, a pointer to a mouse_box struct in .XA.

GETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        ldy     #.sizeof (MOUSE_BOX) - 1

@L1:    lda     XMin,y
        sta     (ptr1),y
        dey
        bpl     @L1

        rts

;----------------------------------------------------------------------------
; MOVE: Move the mouse to a new position. The position is passed as it comes
; from the C program, that is: x on the stack and y in .XA. The C wrapper will
; remove the parameter from the stack, on return.
; No checks are done if the new position is valid (within the bounding box or
; the screen). No return code required.
;

MOVE:   sei                             ; No interrupts

        jsr     MoveY                   ; Set new y position

        ldy     #1
        lda     (sp),y
        sta     XPos+1
        tax
        dey
        lda     (sp),y
        jsr     MoveX                   ; Move the pointer

        cli                             ; Allow interrupts
        rts

;----------------------------------------------------------------------------
; BUTTONS: Return the button mask in .XA.
; Joystick 2's fire button is the  left mouse button.
; Joystick 1's fire button is the right mouse button.

BUTTONS:
        ldx     #15                    ; Switch to the system bank
        stx     IndReg

; Get the fire-button bits

        ldy     #CIA::PRA
        lda     (cia2),y

; Switch back to the execution bank.

        ldy     ExecReg
        sty     IndReg

; Joystick 2, fire button is in bit 7.
; Joystick 1, fire button is in bit 6.

        and     #%11000000
        asl     a                       ; Move bits 7,6 to bits 1,0
        rol     a
        rol     a
        adc     #%00001110              ; Move bit 1 to bit 4
        and     #MOUSE_BTN_LEFT | MOUSE_BTN_RIGHT

; Bits go up when buttons go down.

        eor     #MOUSE_BTN_LEFT | MOUSE_BTN_RIGHT
        ldx     #>$0000
        rts

;----------------------------------------------------------------------------
; INFO: Returns mouse position and current button mask in the MOUSE_INFO
; struct pointed to by ptr1. No return code required.
;
; We're cheating here to keep the code smaller: The first fields of the
; mouse_info struct are identical to the mouse_pos struct; so, we just will
; use _mouse_pos to fill the position fields.

INFO:   jsr     BUTTONS

; Fill in the button state.

        ldy     #MOUSE_INFO::BUTTONS
        sta     (ptr1),y

;       jmp     POS                     ; Fall through

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
; IOCTL: Driver-defined entry point. The wrapper will pass a pointer to ioctl-
; specific data in ptr1, and the ioctl code in .A.
; Must return an error code in .XA.
;

IOCTL:  lda     #<MOUSE_ERR_INV_IOCTL   ; We don't support ioctls, for now
        ldx     #>MOUSE_ERR_INV_IOCTL
        rts

;----------------------------------------------------------------------------
; IRQ: Irq.-handler entry point. Called as a subroutine, but in the IRQ context
; (so, be careful). The routine MUST return carry set if the interrupt has been
; 'handled' -- which means that the interrupt source is gone. Otherwise, it
; MUST return carry clear.
; Reads joystick 2.
;

IRQ:    jsr     CPREP
        ldy     #15                     ; Switch to the system bank
        sty     IndReg

; Get the direction bits.

        ldy     #CIA::PRB
        lda     (cia2),y                ; Read joystick inputs

; Switch back to the execution bank.

        ldy     ExecReg
        sty     IndReg

; Joystick 2, directions in bits 7-4.
; Mask the relevant bits.

        and     #$F0
        eor     #$F0                    ; All bits are inverted
        sta     Temp

; Check left/right.

        and     #(JOY::LEFT | JOY::RIGHT) << 4
        bze     @SkipX

; We will cheat here, and rely on the fact that either the left OR the right
; bit can be active.

        and     #JOY::RIGHT << 4        ; Check RIGHT bit
        bnz     @Right
        lda     #<-$0001
        tax
        bnz     @AddX                   ; Branch always
@Right: lda     #<$0001
        ldx     #>$0001

; Calculate the new x co-ordinate (--> .YA).

@AddX:  add     XPos
        tay                             ; Remember low byte
        txa
        adc     XPos+1
        tax

; Limit the x co-ordinate to the bounding box.

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
@L2:    tya
        jsr     MoveX

; Calculate the y movement vector.

@SkipX: lda     Temp                    ; Get joystick again
        and     #(JOY::UP | JOY::DOWN) << 4  ; Check up/down
        bze     @SkipY

; We will cheat here, and rely on the fact that either the up OR the down
; bit can be active.

        and     #JOY::UP << 4           ; Check UP bit
        bze     @Down
        lda     #<-$0001
        tax
        bnz     @AddY
@Down:  lda     #<$0001
        ldx     #>$0001

; Calculate the new y co-ordinate (--> .YA).

@AddY:  add     YPos
        tay                             ; Remember low byte
        txa
        adc     YPos+1
        tax

; Limit the y co-ordinate to the bounding box.

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
@L4:    tya
        jsr     MoveY

; Done

@SkipY: jsr     CDRAW
        clc                             ; Interrupt not "handled"
        rts

; Move the mouse pointer to the new x pos.

MoveX:  sta     XPos
        stx     XPos+1
        jmp     CMOVEX

; Move the mouse pointer to the new y pos.

MoveY:  sta     YPos
        stx     YPos+1
        jmp     CMOVEY
