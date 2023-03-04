;
; Standard joystick driver for the Atari 7800.
; This version tries to use 7800 and 2600 joysticks.
; But assumes that both joysticks are of same type.
;
; Modified by Karri Kaksonen, 2022-03-31
; Ullrich von Bassewitz, 2002-12-20
; Using code from Steve Schmidtke
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atari7800.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _atari7800_stdjoy_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 2             ; Number of joysticks we support

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        ; Assume 7800 2-button controller, can change
        ; to 2600 1-button later
        lda #$14
        sta CTLSWB ; enable 2-button 7800 controller 1: set pin 6 to output
        ldy #$00
        sty SWCHB ; enable 2-button 7800 controller 2: pull pin 6 (INPT4) high

reset:
        lda #JOY_ERR_OK
        .assert JOY_ERR_OK = 0, error
        tax
;       rts                     ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; COUNT: Return the total number of available joysticks in a/x.
;

COUNT:
        lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A for 2 fire buttons.

readbuttons:
    ; Y has joystick of interest 0/1
    ; return value:
    ;  $00: no button,
    ;  $01: left/B button,
    ;  $02: right/A button,
    ;  $03: both buttons
    ; preserves X
    tya
    beq L5
    ; Joystick 1 processing
    ; 7800 joystick 1 buttons
    ldy #0      ; ........
    bit INPT2   ; Check for right button
    bpl L1
    ldy #2      ; ......2.
L1: bit INPT3   ;Check for left button
    bpl L2
    iny         ; ......21
L2: tya
    bne L4      ; 7800 mode joystick worked
    ; 2600 Joystick 1
    bit INPT5
    bmi L4
L3: iny         ; .......1
    lda #0      ; Fallback to 2600 joystick mode
    sta CTLSWB
L4: tya         ; ......21
    rts

L5: ; Joystick 0 processing
    ; 7800 joystick 0 buttons
    ldy #0      ; ........
    bit INPT0   ; Check for right button
    bpl L6
    ldy #2      ; ......2.
L6: bit INPT1   ;Check for left button
    bpl L7
    iny         ; ......21
L7: tya
    bne L4      ; 7800 mode joystick worked
    ; 2600 Joystick 0
    bit INPT4
    bmi L4
    bpl L3

READ:
    tay         ; Store joystick 0/1 in Y
    beq L8
    lda SWCHA   ; Read directions of joystick 1
    rol         ; ...RLDU.
    rol         ; ..RLDU..
    rol         ; .RLDU... - joystick 1
    jmp L9
L8: lda SWCHA   ; Read directions of joystick 0
    ror         ; .RLDU... - joystick 0
L9: tax
    jsr readbuttons ; A = ......21, X = .RLDU...
    ror             ; A = .......2 1
    tay             ; Y = .......2
    txa             ; A = .RLDU...
    ror             ; A = 1.RLDU..
    tax             ; X = 1.RLDU..
    tya             ; A = .......2
    ror             ; A = ........ 2
    txa             ; A = 1.RLDU..
    rol             ; A = .RLDU..2 1
    rol             ; A = RLDU..21
    eor #$F0        ; The direction buttons were inversed
    and #$F3
    rts

