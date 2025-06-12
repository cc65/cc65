;
; Driver for the Commander X16 Kernal's mouse driver.
;
; 2022-03-28, Greg King
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "cx16.inc"
        .include        "cbm_kernal.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cx16_std_mou

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

        .byte   $00                     ; Don't need interrupts

; Callback table, set by the mouse kernel before INSTALL is called

CHIDE:  jmp     $0000                   ; Hide the cursor
CSHOW:  jmp     $0000                   ; Show the cursor
CPREP:  jmp     $0000                   ; Prepare to move the cursor
CDRAW:  jmp     $0000                   ; Draw the cursor
CMOVEX: jmp     $0000                   ; Move the cursor to X coord
CMOVEY: jmp     $0000                   ; Move the cursor to Y coord


;----------------------------------------------------------------------------
; Constants

SCREEN_WIDTH    = 640 - 1               ; (origin is zero)
SCREEN_HEIGHT   = 480 - 1

;----------------------------------------------------------------------------
; Global variables.

XPos            := ptr3                 ; Current mouse position, X
YPos            := ptr4                 ; Current mouse position, Y

.bss

Box:
XMin:           .res    2               ; X1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box

.rodata

; Default values for above variables
; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefBox
        .word   0                       ; XMin
        .word   SCREEN_WIDTH            ; XMax
        .word   0                       ; YMin
        .word   SCREEN_HEIGHT           ; YMax
.endproc

; These button masks are compatible with the CBM 1351 and the CMD SmartMouse.

ButtMask:
        .byte   %00000000               ; No buttons
        .byte   %00010000               ; Left
        .byte   %00000001               ; Right
        .byte   %00010001               ; Left, right
        .byte   %00000010               ; Middle
        .byte   %00010010               ; Left, middle
        .byte   %00000011               ; Middle, right
        .byte   %00010011               ; Left, middle, right

.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory.
; If possible, check if the hardware is present.
; Must return a MOUSE_ERR_xx code in .XA .

INSTALL:

; Initialize variables. Just copy the default stuff over.

        ldx     #.sizeof(DefBox) - 1
@L1:    lda     DefBox,x
        sta     Box,x
        dex
        bpl     @L1

        sec                     ; Get screen geometry
        jsr     SCREEN_MODE
        lda     #$01            ; Create sprite
        jsr     MOUSE_CONFIG

; Be sure the mouse cursor is invisible, and at the default location. We
; need to do that here, because the mouse interrupt handler might not set
; the mouse position if it hasn't changed.

        jsr     CHIDE
.if 0
        lda     XPos
        ldx     XPos+1
        jsr     CMOVEX
        lda     YPos
        ldx     YPos+1
        jsr     CMOVEY
.endif

; Done, return zero

        ldx     #MOUSE_ERR_OK
        .assert MOUSE_ERR_OK = 0, error
        txa
        rts

;----------------------------------------------------------------------------
; UNINSTALL routine -- is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).

UNINSTALL:                      ; Disable mouse on exit
        lda     #$00
        tax
        jmp     MOUSE_CONFIG

;----------------------------------------------------------------------------
; HIDE routine -- is called to hide the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is called only
; if the mouse currently is visible, and should get hidden. For most drivers,
; no special action is required besides disabling the mouse cursor.
; No return code required.

HIDE:   jmp     CHIDE

;----------------------------------------------------------------------------
; SHOW routine -- is called to show the mouse pointer. The mouse kernel manages
; a counter for calls to show/hide, and the driver entry point is called only
; if the mouse currently is hidden, and should become visible. For most drivers,
; no special action is required besides enabling the mouse cursor.
; No return code required.

SHOW:   jmp     CSHOW

;----------------------------------------------------------------------------
; SETBOX: Set the mouse bounding box. The parameters are passed as they come
; from the C program, that is, a pointer to a mouse_box struct in .XA .
; No checks are done if the mouse is currently inside the box, that is the job
; of the caller. It is not necessary to validate the parameters, trust the
; caller, and save some code here. No return code required.

SETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        lda     (ptr1)
        ldy     #$01

        php
        sei
        sta     XMin
        lda     (ptr1),y
        sta     YMin
        iny
        lda     (ptr1),y
        sta     XMax
        iny
        lda     (ptr1),y
        sta     YMax
        plp

        rts

;; Note: SETBOX and GETBOX currently have no effect!

;----------------------------------------------------------------------------
; GETBOX: Return the mouse bounding box. The parameters are passed as they
; come from the C program, that is, a pointer to a mouse_box struct in .XA .

GETBOX: sta     ptr1
        stx     ptr1+1                  ; Save data pointer

        lda     XMin
        sta     (ptr1)
        ldy     #$01
        lda     YMin
        sta     (ptr1),y
        iny
        lda     XMax
        sta     (ptr1),y
        iny
        lda     YMax
        sta     (ptr1),y
        rts

;----------------------------------------------------------------------------
; MOVE: Put the mouse at a new position. That position is passed as it comes
; from the C program, that is: X on the stack and Y in .XA .  The C wrapper
; will remove the parameter from the stack, on return.
; No checks are done to see if the new position is valid (within
; the bounding box or the screen). No return code required.

;; Note: This function currently has no effect!

MOVE:   php
        sei                             ; No interrupts

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

        plp                             ; Allow interrupts
        rts

;----------------------------------------------------------------------------
; BUTTONS: Return the CBM 1351 button mask in .XA .

BUTTONS:
        ldx     #XPos
        jsr     MOUSE_GET

        and     #%00000111
        tax
        lda     ButtMask,x
        ldx     #>$0000
        rts

;----------------------------------------------------------------------------
; POS: Return the mouse position in the MOUSE_POS struct pointed to by ptr1.
; No return code required.

POS:    jsr     BUTTONS

POS1:   ldy     #MOUSE_POS::XCOORD      ; Structure offset
        lda     XPos                    ; Transfer the position
        sta     (ptr1),y
        lda     XPos+1
        iny
        sta     (ptr1),y
        lda     YPos
        iny
        sta     (ptr1),y
        lda     YPos+1
        iny
        sta     (ptr1),y                ; Store last byte
        rts                             ; Done

;----------------------------------------------------------------------------
; INFO: Returns mouse position and current button mask in the MOUSE_INFO
; struct pointed to by ptr1. No return code required.
;
; We're cheating here to keep the code smaller: The first fields of the
; mouse_info struct are identical to the mouse_pos struct; so, we just will
; call mouse_pos to initialize the struct pointer, and fill the position
; fields.

INFO:   jsr     BUTTONS                 ; Will not touch ptr1
        ldy     #MOUSE_INFO::BUTTONS
        sta     (ptr1),y
        jmp     POS1

;----------------------------------------------------------------------------
; IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an error code in .XA .

IOCTL:  lda     #MOUSE_ERR_INV_IOCTL    ; We don't support ioctls, for now
        ldx     #0 ; return value is char
;       rts                             ; Fall through

;----------------------------------------------------------------------------
; IRQ: Irq handler entry point. Called as a subroutine but in IRQ context
; (so be careful). The routine MUST return carry set if the interrupt has been
; 'handled' -- which means that the interrupt source is gone. Otherwise, it
; MUST return carry clear.

IRQ:    rts                             ; Kernal ROM does this routine's job
