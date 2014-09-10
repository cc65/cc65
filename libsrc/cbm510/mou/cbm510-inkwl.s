;
; Driver for the Inkwell Systems 170-C and 184-C lightpens.
;
; This driver reads only the main button on the 184-C.
;
; 2014-09-10, Greg King
;

        .include        "zeropage.inc"
        .include        "../extzp.inc"

        .include        "mouse-kernel.inc"
        .include        "cbm510.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table.

        module_header   _cbm510_inkwl_mou

HEADER:

; Driver signature

        .byte   $6d, $6f, $75           ; ASCII "mou"
        .byte   MOUSE_API_VERSION       ; Mouse driver API version number

; Library reference

LIBREF: .addr   $0000

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

        .byte   MOUSE_FLAG_EARLY_IRQ

; Callback table, set by the kernel before INSTALL is called.

CHIDE:  jmp     $0000                   ; Hide the cursor
CSHOW:  jmp     $0000                   ; Show the cursor
CPREP:  jmp     $0000                   ; Prepare to move the cursor
CDRAW:  jmp     $0000                   ; Draw the cursor
CMOVEX: jmp     $0000                   ; Move the cursor to X co-ord.
CMOVEY: jmp     $0000                   ; Move the cursor to Y co-ord.


;----------------------------------------------------------------------------
; Constants

SCREEN_WIDTH    = XSIZE * 8
SCREEN_HEIGHT   = YSIZE * 8

;----------------------------------------------------------------------------
; Global variables. The bounding box values are sorted so that they can be
; written with the least effort in the SETBOX and GETBOX routines; so, don't
; re-order them.

.rodata

; Default values for below variables
; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefVars
        .word   0                       ; XMin
        .word   0                       ; YMin
        .word   SCREEN_WIDTH - 1        ; XMax
        .word   SCREEN_HEIGHT - 1       ; YMax
.endproc

.bss

Vars:
XMin:           .res    2               ; X1 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box

XPos:           .res    2               ; Current lightpen position, X
YPos:           .res    2               ; Current lightpen position, Y

OldPenX:        .res    1               ; Previous HW-counter values
OldPenY:        .res    1

.data

; Default Inkwell calibration.
; The first number is the width of the left border;
; the second number is the actual calibration value.
; See a comment below (at "Calculate the new X co-ordinate")
; for the reason for the third number.

XOffset:        .byte   (24 + 24) / 2   ; x-offset

; Jump to a function that puts a new calibration value into XOffset.
Calibrate:      jmp     $0000


.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return a MOUSE_ERR_xx code in .XA.

INSTALL:

; Initiate variables. Just copy the default stuff over.

        ldx     #.sizeof (DefVars) - 1
@L0:    lda     DefVars,x
        sta     Vars,x
        dex
        bpl     @L0

        ldx     #15                    ; Change to system bank
        stx     IndReg
        ldy     #VIC_LPEN_X
        lda     (vic),y
        sta     OldPenX
        ldy     #VIC_LPEN_Y
        lda     (vic),y
        sta     OldPenY
        ldx     ExecReg                 ; Change back to execution bank
        stx     IndReg

; Call a calibration function through the library-reference.

        lda     LIBREF
        ldx     LIBREF+1
        sta     ptr1                    ; Point to mouse_adjuster
        stx     ptr1+1
        ldy     #1
        lda     (ptr1),y
        bze     @L1                     ; Don't call pointer if it's NULL
        sta     Calibrate+2             ; Point to function
        dey
        lda     (ptr1),y
        sta     Calibrate+1
        lda     #<XOffset               ; Function will set this variable
        ldx     #>XOffset
        jsr     Calibrate

; Be sure that the lightpen cursor is invisible and at the default location.
; It needs to be done here because the lightpen interrupt handler doesn't
; set the lightpen position if it hasn't changed.

@L1:    sei
        jsr     CHIDE

        lda     #<(SCREEN_HEIGHT / 2)
        ldx     #>(SCREEN_HEIGHT / 2)
        jsr     MoveY
        lda     #<(SCREEN_WIDTH / 2)
        ldx     #>(SCREEN_WIDTH / 2)
        jsr     MoveX
        cli

; Done, return zero.

        lda     #MOUSE_ERR_OK
        tax
        rts

;----------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).

UNINSTALL       := HIDE                 ; Hide cursor on exit

;----------------------------------------------------------------------------
; HIDE routine. Is called to hide the lightpen pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is called only
; if the mouse is currently visible, and should get hidden. For most drivers,
; no special action is required besides hiding the lightpen cursor.
; No return code required.

HIDE:   sei
        jsr     CHIDE
        cli
        rts

;----------------------------------------------------------------------------
; SHOW routine. Is called to show the lightpen pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is called only
; if the mouse is currently hidden, and should become visible. For most drivers,
; no special action is required besides enabling the lightpen cursor.
; No return code required.

SHOW:   sei
        jsr     CSHOW
        cli
        rts

;----------------------------------------------------------------------------
; SETBOX: Set the lightpen bounding box. The parameters are passed as they come
; from the C program, that is, a pointer to a mouse_box struct in .XA.
; No checks are done if the lightpen is currently inside the box, that is the job
; of the caller. It is not necessary to validate the parameters; trust the
; caller; and, save some code here. No return code required.

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
; GETBOX: Return the lightpen bounding box. The parameters are passed as they
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
; from the C program, that is: X on the stack and Y in .XA. The C wrapper will
; remove the parameter from the stack on return.
; No checks are done if the new position is valid (within the bounding box or
; the screen). No return code required.
;

MOVE:   sei                             ; No interrupts
        jsr     MoveY

        ldy     #$01
        lda     (sp),y
        tax
        dey
        lda     (sp),y
        jsr     MoveX                   ; Move the cursor

        cli                             ; Allow interrupts
        rts

;----------------------------------------------------------------------------
; BUTTONS: Return the button mask in .XA.

BUTTONS:
        ldx     #15                     ; To system bank
        stx     IndReg
        ldy     #CIA::PRB
        lda     (cia2),y                ; Read joystick inputs
        ldx     ExecReg                 ; Back to execution bank
        stx     IndReg

; Joystick 1, directions in bits 3-0.
; Make the lightpen button look like a 1351 mouse.

        asl     a                       ; Move joystick-left bit ...
        asl     a                       ; ... to fire-button bit
        eor     #MOUSE_BTN_LEFT
        and     #MOUSE_BTN_LEFT
        ldx     #>$0000
        rts

;----------------------------------------------------------------------------
; POS: Return the lightpen position in the MOUSE_POS struct pointed to by ptr1.
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
        rts

;----------------------------------------------------------------------------
; INFO: Returns lightpen position and current button mask in the MOUSE_INFO
; struct pointed to by ptr1. No return code required.
;
; We're cheating here, to keep the code smaller: The first fields of the
; mouse_info struct are identical to the mouse_pos struct; so, we'll just
; call _mouse_pos to initiate the struct pointer, and fill the position
; fields.

INFO:   jsr     POS

; Fill in the button state

        jsr     BUTTONS                 ; Will not touch ptr1
        ldy     #MOUSE_INFO::BUTTONS
        sta     (ptr1),y
        rts

;----------------------------------------------------------------------------
; IOCTL: Driver-defined entry point. The wrapper will pass a pointer to ioctl-
; specific data in ptr1, and the ioctl code in .A.
; Must return an error code in .XA.
;

IOCTL:  lda     #<MOUSE_ERR_INV_IOCTL     ; We don't support ioctls, for now
        ldx     #>MOUSE_ERR_INV_IOCTL
        rts

;----------------------------------------------------------------------------
; IRQ: Irq.-handler entry point. Called as a subroutine, but in the IRQ context
; (so, be careful). The routine MUST return carry set if the interrupt has been
; 'handled' -- which means that the interrupt source is gone. Otherwise, it
; MUST return carry clear.
;

IRQ:    jsr     CPREP
        ldx     #15                     ; To system bank
        stx     IndReg

; Read the VIC-II lightpen registers.

        ldy     #VIC_LPEN_Y
        lda     (vic),y
        cmp     OldPenY

; Skip processing if nothing has changed.

        beq     @SkipY
        sta     OldPenY
        ldx     ExecReg                 ; Back to execution bank
        stx     IndReg

; Subtract the height of the top border, so that the lightpen co-ordinate
; will match the TGI co-ordinate.

        sub     #50
        tay                             ; Remember low byte
        ldx     #>$0000

; Limit the Y co-ordinate to the bounding box.

        txa
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

        ldx     #15                     ; To system bank
        stx     IndReg
@SkipY: ldy     #VIC_LPEN_X
        lda     (vic),y
        ldx     ExecReg                 ; Back to execution bank
        stx     IndReg
        cmp     OldPenX

; Skip processing if nothing has changed.

        beq     @SkipX
        sta     OldPenX

; Adjust the value by the calibration offset.

        sub     XOffset

; Calculate the new X co-ordinate.
; The VIC-II register is eight bits; but, the screen co-ordinate is nine bits.
; Therefore, the VIC-II number is doubled. Then, it points to every other pixel;
; but, it can reach across the screen.

        asl     a
        tay                             ; Remember low byte
        lda     #>$0000
        rol     a
        tax                             ; Remember high byte

; Limit the X co-ordinate to the bounding box.

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

; Done

@SkipX: jsr     CDRAW
        clc                             ; Interrupt not "handled"
        rts

; Move the lightpen pointer to the new Y pos.

MoveY:  sta     YPos
        stx     YPos+1
        jmp     CMOVEY

; Move the lightpen pointer to the new X pos.

MoveX:  sta     XPos
        stx     XPos+1
        jmp     CMOVEX
