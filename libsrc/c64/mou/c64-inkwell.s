;
; Driver for the Inkwell Systems 170-C and 184-C lightpens.
;
; 2013-05-16, Greg King
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "c64.inc"

        .macpack        generic
        .macpack        cbm

; ------------------------------------------------------------------------
; Header. Includes jump table.

.segment        "JUMPTABLE"

HEADER:

; Driver signature

        .byte   $6d, $6f, $75           ; ASCII "mou"
        .byte   MOUSE_API_VERSION       ; Mouse driver API version number

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
CMOVEX: jmp     $0000                   ; Move the cursor to X co-ord.
CMOVEY: jmp     $0000                   ; Move the cursor to Y co-ord.


;----------------------------------------------------------------------------
; Constants

SCREEN_HEIGHT   = YSIZE * 8
SCREEN_WIDTH    = XSIZE * 8
SCREEN_ZONE     = YSIZE / 4 * XSIZE

; This driver is for the standard 40-column screen.

SCREEN          := $0400
COLOR_RAM       := $D800


;----------------------------------------------------------------------------
; Global variables. The bounding box values are sorted so that they can be
; written with the least effort in the SETBOX and GETBOX routines; so, don't
; re-order them.

.bss

Vars:
XMin:           .res    2               ; X1 value of bounding box
YMin:           .res    2               ; Y1 value of bounding box
XMax:           .res    2               ; X2 value of bounding box
YMax:           .res    2               ; Y2 value of bounding box
Buttons:        .res    1               ; Button status bits
XPos:           .res    2               ; Current lightpen position, X
YPos:           .res    2               ; Current lightpen position, Y

OldPenX:        .res    1               ; Old HW-counter values
OldPenY:        .res    1

.data

; Start with an average offset.

XOffset:        .byte   48 / 2          ; Calibration offset, for X position

.rodata

; Default values for above variables
; (We use ".proc" because we want to define both a label and a scope.)

.proc   DefVars
        .word   0                       ; XMin
        .word   0                       ; YMin
        .word   SCREEN_WIDTH - 1        ; XMax
        .word   SCREEN_HEIGHT - 1       ; YMax
        .byte   %00000000               ; Buttons
.endproc

.proc   Command1
        scrcode "Adjust by clicking on line."
.endproc
.proc   Command2
        scrcode "Finish by clicking off box."
.endproc


.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return a MOUSE_ERR_xx code in .XA.

INSTALL:

; Initiate variables. Just copy the default stuff over.

        sei
        ldx     #.sizeof (DefVars) - 1
@L0:    lda     DefVars,x
        sta     Vars,x
        dex
        bpl     @L0

        ldx     VIC_LPEN_X
        ldy     VIC_LPEN_Y
        stx     OldPenX
        sty     OldPenY
        cli

; There is a delay between when the VIC sends its signal, and when the display
; shows that signal.  There is another delay between the display and when
; the lightpen says that it saw that signal. Each display and pen is different.
; Therefore, this driver must be calibrated to them.  A white box is painted on
; the screen; and, a line is drawn down the middle of it.  When the user clicks
; on that line, the difference between its position and where the VIC thinks
; that the pen is pointing becomes an offset that is subtracted from what the
; VIC sees.

        lda     VIC_BG_COLOR0
        ldx     #6                      ; Blue screen
        stx     VIC_BG_COLOR0
        pha
        jsr     CLRSCR

        ldy     #.sizeof (Command2) - 1
@L2:    lda     Command2,y
        sta     SCREEN + SCREEN_ZONE * 2 + XSIZE * 3 + (XSIZE - .sizeof (Command2)) / 2,y
        lda     #15                     ; Light gray text
        sta     COLOR_RAM + SCREEN_ZONE * 2 + XSIZE * 3 + (XSIZE - .sizeof (Command1)) / 2,y
        dey
        bpl     @L2
        ldy     #.sizeof (Command1) - 1
@L1:    lda     Command1,y
        sta     SCREEN + SCREEN_ZONE * 2 + XSIZE * 1 + (XSIZE - .sizeof (Command1)) / 2,y
        lda     #15                     ; Light gray text
        sta     COLOR_RAM + SCREEN_ZONE * 2 + XSIZE * 1 + (XSIZE - .sizeof (Command1)) / 2,y
        dey
        bpl     @L1

        ldx     #SCREEN_ZONE
@L3:    lda     #$80 | $20              ; Reversed space screen-code
        sta     SCREEN + SCREEN_ZONE - 1,x
        lda     #15                     ; Light gray box
        sta     COLOR_RAM + SCREEN_ZONE - 1,x
        dex
        bnz     @L3

        ldy     #$80 | $5d              ; Reversed vertical-bar screen-code
        .repeat 4, L
        sty     SCREEN + SCREEN_ZONE + (L + 1) * XSIZE + XSIZE / 2
        .endrep

        lda     VIC_SPR0_COLOR
        ldx     #12                     ; Medium gray pointer
        stx     VIC_SPR0_COLOR
        pha
        jsr     SHOW

        lda     #<(SCREEN_HEIGHT / 4 / 2)
        ldx     #>(SCREEN_HEIGHT / 4 / 2)
        jsr     PutCursor

; Wait for the main button to be released.

@L4:    lda     Buttons
        bnz     @L4

; Wait for the main button to be pressed.

@L5:    lda     Buttons
        bze     @L5

; Find out if the pen is on or off the box.

        ldy     YPos
        ldx     YPos+1
        txa
        cpy     #<(YSIZE / 4 * 1 * 8)
        sbc     #>(YSIZE / 4 * 1 * 8)
        bmi     @L6                     ; Above box
        txa
        cpy     #<(YSIZE / 4 * 2 * 8)
        sbc     #>(YSIZE / 4 * 2 * 8)
        bpl     @L6                     ; Below box

; The pen is on the box; adjust the offset.

        lda     OldPenX
        sub     #(XSIZE * 8 / 2 + 8/2) / 2
        sta     XOffset
        sta     OldPenX                 ; Make IRQ update X co-ordinate
        jmp     @L4

; Wait for the main button to be released.

@L6:    lda     Buttons
        bnz     @L6

        lda     XOffset                 ; Tell test program about calibration
        sta     $3ff

        pla
        sta     VIC_SPR0_COLOR
        pla
        sta     VIC_BG_COLOR0
        jsr     CLRSCR

; Be sure the lightpen cursor is invisible and at the default location.
; It needs to be done here because the lightpen interrupt handler doesn't
; set the lightpen position if it hasn't changed.

        jsr     HIDE

        lda     #<(SCREEN_HEIGHT / 2)
        ldx     #>(SCREEN_HEIGHT / 2)
PutCursor:
        sei
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
        lda     Buttons
        ldx     #>0

; Make the lightpen buttons look like a 1351 mouse.

        asl     a
        asl     SID_ADConv2             ; PotY
        rol     a
        eor     #MOUSE_BTN_RIGHT
        and     #MOUSE_BTN_LEFT | MOUSE_BTN_RIGHT
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
; IRQ: IRQ handler entry point. Called as a subroutine, but in the IRQ context
; (so, be careful). The routine MUST return carry set if the interrupt has been
; 'handled' -- which means that the interrupt source is gone. Otherwise, it
; MUST return carry clear.
;

IRQ:

; Record the state of the buttons.
; Try to avoid crosstalk between the keyboard and the lightpen.

        ldy     #%00000000              ; Set ports A and B to input
        sty     CIA1_DDRB
        sty     CIA1_DDRA               ; Keyboard won't look like buttons
        lda     CIA1_PRB                ; Read Control-Port 1
        dec     CIA1_DDRA               ; Set port A back to output
        eor     #%11111111              ; Bit goes up when button goes down
        sta     Buttons
        bze     @L0
        lda     #%11101111              ; (Don't change bit that feeds VIC-II)
        sta     CIA1_DDRB               ; Buttons won't look like keyboard
        sty     CIA1_PRB                ; Set "all keys pushed"

; Read the VIC-II lightpen registers.

@L0:    lda     VIC_LPEN_Y
        cmp     OldPenY

; Skip processing if nothing has changed.

        beq     @SkipY
        sta     OldPenY

; Subtract the height of the top border, so that the lightpen co-ordinate
; will match the TGI co-ordinate.

        sub     #50
        tay                             ; Remember low byte
        ldx     #>0

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

@SkipY: lda     VIC_LPEN_X
        cmp     OldPenX

; Skip processing if nothing has changed.

        beq     @SkipX
        sta     OldPenX

; Adjust the co-ordinate by the calibration offset.

        sub     XOffset

; Calculate the new X co-ordinate (--> .AY --> .XY).
; The VIC-II register is eight bits; but, the screen co-ordinate is nine bits.
; Therefor, the VIC-II number is doubled. Then, it points to every other pixel;
; but, it can reach across the screen.

        asl     a
        tay                             ; Remember low byte
        lda     #>0
        rol     a
        tax

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

@SkipX: clc                             ; Interrupt not "handled"
        rts

; Move the lightpen pointer to the new Y pos.

MoveY:  sta     YPos
        stx     YPos+1
        jmp     CMOVEY

; Move the lightpen pointer to the new X pos.

MoveX:  sta     XPos
        stx     XPos+1
        jmp     CMOVEX
