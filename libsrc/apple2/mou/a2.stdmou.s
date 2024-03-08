;
; Driver for the AppleMouse II Card.
;
; Oliver Schmidt, 03.09.2005
;

        .include        "zeropage.inc"
        .include        "mouse-kernel.inc"
        .include        "apple2.inc"

        .macpack        module

; ------------------------------------------------------------------------

SETMOUSE        = $12   ; Sets mouse mode
SERVEMOUSE      = $13   ; Services mouse interrupt
READMOUSE       = $14   ; Reads mouse position
CLEARMOUSE      = $15   ; Clears mouse position to 0 (for delta mode)
POSMOUSE        = $16   ; Sets mouse position to a user-defined pos
CLAMPMOUSE      = $17   ; Sets mouse bounds in a window
HOMEMOUSE       = $18   ; Sets mouse to upper-left corner of clamp win
INITMOUSE       = $19   ; Resets mouse clamps to default values and
                        ; sets mouse position to 0,0

pos1_lo         := $0478
pos1_hi         := $0578
pos2_lo         := $04F8
pos2_hi         := $05F8
status          := $0778

; ------------------------------------------------------------------------

        .ifdef  __APPLE2ENH__
        module_header   _a2e_stdmou_mou
        .else
        module_header   _a2_stdmou_mou
        .endif

        ; Driver signature
        .byte   $6D, $6F, $75           ; "mou"
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
        .byte   MOUSE_FLAG_EARLY_IRQ

        ; Callback table, set by the kernel before INSTALL is called
CHIDE:  jmp     $0000                   ; Hide the cursor
CSHOW:  jmp     $0000                   ; Show the cursor
CPREP:  jmp     $0000                   ; Prepare to move the cursor
CDRAW:  jmp     $0000                   ; Draw the cursor
CMOVEX: jmp     $0000                   ; Move the cursor to X coord
CMOVEY: jmp     $0000                   ; Move the cursor to Y coord

; ------------------------------------------------------------------------

        .bss

box:    .tag    MOUSE_BOX
info:   .tag    MOUSE_INFO
slot:   .res    1

; ------------------------------------------------------------------------

        .rodata

offsets:.byte   $05             ; Pascal 1.0 ID byte
        .byte   $07             ; Pascal 1.0 ID byte
        .byte   $0B             ; Pascal 1.1 generic signature byte
        .byte   $0C             ; Device signature byte

values: .byte   $38             ; Fixed
        .byte   $18             ; Fixed
        .byte   $01             ; Fixed
        .byte   $20             ; X-Y pointing device type 0

size    = * - values

inibox: .word     0             ; MinX
        .word     0             ; MinY
        .word   279             ; MaxX
        .word   191             ; MaxY

; ------------------------------------------------------------------------

        .data

firmware:
        ; Lookup and patch firmware address lobyte
lookup: ldy     $FF00,x         ; Patched at runtime
        sty     jump+1          ; Modify code below

        ; Apple II Mouse TechNote #1, Interrupt Environment with the Mouse:
        ; "Enter all mouse routines (...) with the X register set to $Cn
        ;  and Y register set to $n0, where n = the slot number."
xparam: ldx     #$FF            ; Patched at runtime
yparam: ldy     #$FF            ; Patched at runtime

jump:   jmp     $FFFF           ; Patched at runtime

; ------------------------------------------------------------------------

        .code

; INSTALL: Is called after the driver is loaded into memory. If possible,
; check if the hardware is present. Must return an MOUSE_ERR_xx code in A/X.
INSTALL:
        lda     #<$C000
        sta     ptr1
        lda     #>$C000
        sta     ptr1+1

        ; Search for AppleMouse II firmware in slots 1 - 7
next:   inc     ptr1+1
        lda     ptr1+1
        cmp     #>$C800
        bcc     :+

        ; Mouse firmware not found
        lda     #MOUSE_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

        ; Check Pascal 1.1 Firmware Protocol ID bytes
:       ldx     #size - 1
:       ldy     offsets,x
        lda     values,x
        cmp     (ptr1),y
        bne     next
        dex
        bpl     :-

        ; Get and patch firmware address hibyte
        lda     ptr1+1
        sta     lookup+2
        sta     xparam+1
        sta     jump+2

        ; Disable interrupts now because setting the slot number makes
        ; the IRQ handler (maybe called due to some non-mouse IRQ) try
        ; calling the firmware which isn't correctly set up yet
        sei

        ; Convert to and save slot number
        and     #$0F
        sta     slot

        ; Convert to and patch I/O register index
        asl
        asl
        asl
        asl
        sta     yparam+1

        ; The AppleMouse II Card needs the ROM switched in
        ; to be able to detect an Apple //e and use RDVBL
        bit     $C082

        ; Reset mouse hardware
        ldx     #INITMOUSE
        jsr     firmware

        ; Switch in LC bank 2 for R/O
        bit     $C080

        ; Turn mouse on
        lda     #%00000001
        ldx     #SETMOUSE
        jsr     firmware

        ; Set initial mouse clamps
        lda     #<inibox
        ldx     #>inibox
        jsr     SETBOX

        ; Set initial mouse position
        ldx     slot
        lda     #<(279 / 2)
        sta     pos1_lo,x
        lda     #>(279 / 2)
        sta     pos1_hi,x
        lda     #<(191 / 2)
        sta     pos2_lo,x
        lda     #>(191 / 2)
        sta     pos2_hi,x
        ldx     #POSMOUSE
        jsr     firmware

        ; Update cursor
        jsr     update

        ; Turn VBL interrupt on
        lda     #%00001001
        ldx     #SETMOUSE
common: jsr     firmware

        ; Enable interrupts and return success
        cli
        lda     #<MOUSE_ERR_OK
        ldx     #>MOUSE_ERR_OK
        rts

; UNINSTALL: Is called before the driver is removed from memory.
; No return code required (the driver is removed from memory on return).
UNINSTALL:
        ; Hide cursor
        sei
        jsr     CHIDE

        ; Turn mouse off
        lda     #%00000000
        ldx     #SETMOUSE
        bne     common          ; Branch always

; SETBOX: Set the mouse bounding box. The parameters are passed as they come
; from the C program, that is, a pointer to a mouse_box struct in A/X.
; No checks are done if the mouse is currently inside the box, this is the job
; of the caller. It is not necessary to validate the parameters, trust the
; caller and save some code here. No return code required.
SETBOX:
        sta     ptr1
        stx     ptr1+1

        ; Set x clamps
        ldx     #$00
        ldy     #MOUSE_BOX::MINX
        jsr     :+

        ; Set y clamps
        ldx     #$01
        ldy     #MOUSE_BOX::MINY

        ; Apple II Mouse TechNote #1, Interrupt Environment with the Mouse:
        ; "Disable interrupts before placing position information in the
        ;  screen holes."
:       sei

        ; Set low clamp
        lda     (ptr1),y
        sta     box,y
        sta     pos1_lo
        iny
        lda     (ptr1),y
        sta     box,y
        sta     pos1_hi

        ; Skip one word
        iny
        iny

        ; Set high clamp
        iny
        lda     (ptr1),y
        sta     box,y
        sta     pos2_lo
        iny
        lda     (ptr1),y
        sta     box,y
        sta     pos2_hi

        txa
        ldx     #CLAMPMOUSE
        bne     common          ; Branch always

; GETBOX: Return the mouse bounding box. The parameters are passed as they
; come from the C program, that is, a pointer to a mouse_box struct in A/X.
GETBOX:
        sta     ptr1
        stx     ptr1+1

        ldy     #.sizeof(MOUSE_BOX)-1
:       lda     box,y
        sta     (ptr1),y
        dey
        bpl     :-
        rts

; MOVE: Move the mouse to a new position. The position is passed as it comes
; from the C program, that is: x on the stack and y in A/X. The C wrapper will
; remove the parameter from the stack on return.
; No checks are done if the new position is valid (within the bounding box or
; the screen). No return code required.
MOVE:
        ldy     slot
        sei

        ; Set y
        sta     pos2_lo,y
        txa
        sta     pos2_hi,y

        tya
        tax
        ldy     #$00            ; Start at top of stack

        ; Set x
        lda     (sp),y
        iny
        sta     pos1_lo,x
        lda     (sp),y
        sta     pos1_hi,x

        ; Update cursor
        jsr     update

        ldx     #POSMOUSE
        bne     common          ; Branch always

; HIDE: Is called to hide the mouse cursor. The mouse kernel manages a
; counter for calls to show/hide, and the driver entry point is only called
; if the mouse is currently visible and should get hidden. For most drivers,
; no special action is required besides hiding the mouse cursor.
; No return code required.
HIDE:
        sei
        jsr     CHIDE
        cli
        rts

; SHOW: Is called to show the mouse cursor. The mouse kernel manages a
; counter for calls to show/hide, and the driver entry point is only called
; if the mouse is currently hidden and should become visible. For most drivers,
; no special action is required besides enabling the mouse cursor.
; No return code required.
SHOW:
        sei
        jsr     CSHOW
        cli
        rts

; BUTTONS: Return the button mask in A/X.
BUTTONS:
        lda     info + MOUSE_INFO::BUTTONS
        ldx     #$00
        rts

; POS: Return the mouse position in the MOUSE_POS struct pointed to by ptr1.
; No return code required.
POS:
        ldy     #.sizeof(MOUSE_POS)-1
        bne     copy            ; Branch always

; INFO: Returns mouse position and current button mask in the MOUSE_INFO
; struct pointed to by ptr1. No return code required.
INFO:
        ldy     #.sizeof(MOUSE_INFO)-1
copy:   sei
:       lda     info,y
        sta     (ptr1),y
        dey
        bpl     :-
        cli
        rts

; IOCTL: Driver defined entry point. The wrapper will pass a pointer to ioctl
; specific data in ptr1, and the ioctl code in A.
; Must return an MOUSE_ERR_xx code in A/X.
IOCTL:
        lda     #<MOUSE_ERR_INV_IOCTL
        ldx     #>MOUSE_ERR_INV_IOCTL
        rts

; IRQ: Called from the builtin runtime IRQ handler as a subroutine. All
; registers are already saved, no parameters are passed, but the carry flag
; is clear on entry. The routine must return with carry set if the interrupt
; was handled, otherwise with carry clear.
IRQ:
        ; Check for installed mouse
        lda     slot
        beq     done

        ; Check for mouse interrupt
        ldx     #SERVEMOUSE
        jsr     firmware
        bcc     :+
        clc                     ; Interrupt not handled
done:   rts

:       ldx     #READMOUSE
        jsr     firmware

        ; Get status
        ldy     slot
        lda     status,y
        tax                     ; Save status

        ; Extract button down values
        asl                     ;  C = Button 0 is currently down
        and     #%00100000      ; !Z = Button 1 is currently down

        ; Set button mask
        beq     :+
        lda     #MOUSE_BTN_RIGHT
:       bcc     :+
        ora     #MOUSE_BTN_LEFT
:       sta     info + MOUSE_INFO::BUTTONS

        ; Check for mouse movement
        txa                     ; Restore status
        and     #%00100000      ; X or Y changed since last READMOUSE
        beq     :+

        ; Remove the cursor at the old position
update: jsr     CPREP

        ; Get and set the new X position
        ldy     slot
        lda     pos1_lo,y
        ldx     pos1_hi,y
        sta     info + MOUSE_POS::XCOORD
        stx     info + MOUSE_POS::XCOORD+1
        jsr     CMOVEX

        ; Get and set the new Y position
        ldy     slot
        lda     pos2_lo,y
        ldx     pos2_hi,y
        sta     info + MOUSE_POS::YCOORD
        stx     info + MOUSE_POS::YCOORD+1
        jsr     CMOVEY

        ; Draw the cursor at the new position
:       jsr     CDRAW
        sec                     ; Interrupt handled
        rts
