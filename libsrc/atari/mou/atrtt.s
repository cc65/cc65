;
; Mouse driver for Atari Touch Tablet
;
; Christian Groessler, 2014-01-05
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "atari.inc"

        .macpack        generic
        .macpack        module

; ------------------------------------------------------------------------
; Header. Includes jump table

.ifdef __ATARIXL__
        module_header   _atrxtt_mou
.else
        module_header   _atrtt_mou
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
OldButton:      .res    1               ; previous buttons

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

; Done

        ldx     #MOUSE_ERR_OK
        .assert MOUSE_ERR_OK = 0, error
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

        jsr     CSHOW

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
        stx     Buttons

        lda     PORTA                   ; get other buttons
        eor     #255
        tax
        and     #5                      ; pen button and left button are mapped to left mouse button
        beq     @L01
        lda     #MOUSE_BTN_LEFT
        ora     Buttons
        sta     Buttons
@L01:   txa
        and     #8
        beq     @L02
        lda     #MOUSE_BTN_RIGHT
        ora     Buttons
        sta     Buttons

; Check if button status changed, and disable "attract mode" if yes

@L02:   lda     Buttons
        cmp     OldButton
        beq     @L03
        sta     OldButton
        lda     #0
        sta     ATRACT

; If we read 228 for X or Y positions, we assume the user has lifted the pen
; and don't change the cursor position.

@L03:   lda     PADDL0
        cmp     #228
        beq     @Cont                   ; CF set if equal
        lda     PADDL1
        cmp     #228                    ; CF set if equal

@Cont:  php                             ; remember CF
        jsr     CPREP
        plp                             ; restore CF

        bcc     @L04
        jmp     @Show

@L04:   ldx     #0
        stx     XPos+1
        stx     YPos+1
        stx     ATRACT                  ; disable "attract mode"

; Get cursor position
; -------------------
; The touch pad is read thru the paddle potentiometers. The possible
; values are 1..228. Since the maximum value is less than the X
; dimension we have to "stretch" this value. In order to use only
; divisions by powers of two, we use the following appoximation:
; 320/227 = 1.4096
; 1+1/2-1/8+1/32 = 1.4062
; For Y we subtract 1/8 of it to get in the YMax ballpark.
; 228-228/8=199.5
; A small area in the Y dimension of the touchpad isn't used with
; this approximation. The Y value is inverted, (0,0) is the bottom
; left corner of the touchpad.

; X

        ldx     PADDL0                  ; get X postion
        dex                             ; decrement, since it's 1-based
        stx     XPos
        txa
        lsr     a
        tax
        clc
        adc     XPos
        sta     XPos
        bcc     @L05
        inc     XPos+1
@L05:   txa
        lsr     a                       ; port value / 4
        lsr     a                       ; port value / 8
        tax
        sec
        lda     XPos
        stx     XPos
        sbc     XPos
        sta     XPos
        bcs     @L06
        dec     XPos+1
@L06:   txa
        lsr     a                       ; port value / 16
        lsr     a                       ; port value / 32
        clc
        adc     XPos
        sta     XPos
        bcc     @L07
        inc     XPos+1

@L07:   tay
        lda     XPos+1
        tax

; Limit the X coordinate to the bounding box

        cpy     XMin
        sbc     XMin+1
        bpl     @L08
        ldy     XMin
        ldx     XMin+1
        jmp     @L09
@L08:   txa

        cpy     XMax
        sbc     XMax+1
        bmi     @L09
        ldy     XMax
        ldx     XMax+1
@L09:   sty     XPos
        stx     XPos+1

; Move the mouse pointer to the new X pos

        tya
        jsr     CMOVEX

; Y

        ldx     PADDL1                  ; get Y postion
        dex                             ; decrement, since it's 1-based
        stx     YPos
        lda     #228
        sec
        sbc     YPos                    ; invert value
        tax
        lsr     a
        lsr     a
        lsr     a
        sta     YPos
        txa
        sec
        sbc     YPos
        sta     YPos
        tay
        lda     YPos+1
        tax

; Limit the Y coordinate to the bounding box

        cpy     YMin
        sbc     YMin+1
        bpl     @L10
        ldy     YMin
        ldx     YMin+1
        jmp     @L11
@L10:   txa

        cpy     YMax
        sbc     YMax+1
        bmi     @L11
        ldy     YMax
        ldx     YMax+1
@L11:   sty     YPos
        stx     YPos+1

; Move the mouse pointer to the new X pos

        tya
        jsr     CMOVEY

@Show:  jsr     CDRAW

        clc                             ; Interrupt not "handled"
        rts
