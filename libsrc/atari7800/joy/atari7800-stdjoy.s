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

PB2 = $04 ; Joystick 0
PB4 = $10 ; Joystick 1

INSTALL:
        ; Assume 7800 2-button controller, can change
        ; to 2600 1-button later
        lda #(PB2 | PB4)
        ; enable 2-button 7800 controllers on both ports
        ; by setting PB2 and PB4 to output
        sta CTLSWB
        ; enable 2-button 7800 controllers by setting
        ; the outputs to 0; (INPT4 and INPT5) high
        ldy #$00
        sty SWCHB

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

readdualbuttons0:
    ldy #0      ; ........
    bit INPT0   ; Check for right button
    bpl L1
    ldy #2      ; ......2.
L1: bit INPT1   ; Check for left button
    bpl L2
    iny         ; ......21
L2: tya
    rts

readdualbuttons1:
    ldy #0      ; ........
    bit INPT2   ; Check for right button
    bpl L1
    ldy #2      ; ......2.
L3: bit INPT3   ; Check for left button
    bpl L2
    iny         ; ......21
L4: tya
    rts

readbuttons:
    ; Y has joystick of interest 0/1
    ; return value:
    ;  $00: no button,
    ;  $01: left/B button,
    ;  $02: right/A button,
    ;  $03: both buttons
    ; preserves X
    tya
    beq readbuttons0
readbuttons1:
    ; Joystick 1 processing
    ; Start by checking for single button 2600 joystick
    bit INPT5
    bpl singlebtn1detected
    jmp readdualbuttons1
singlebtn1detected:
    ; Single button joystick detected but could be dual
    jsr readdualbuttons1
    bne L5      ; It was a dual button press
    ; It was a single button press
    bit INPT5
    bmi L5
    iny         ; .......1
    lda #PB4    ; Joystick 1 is a single button unit
    clc
    adc SWCHB
    sta SWCHB   ; Cut power from the dual button circuit
L5: tya         ; ......21
    rts

readbuttons0:
    ; Joystick 0 processing
    ; Start by checking for single button 2600 joystick
    bit INPT4
    bpl singlebtn0detected
    jmp readdualbuttons0
singlebtn0detected:
    ; Single button joystick detected but could be dual
    jsr readdualbuttons0
    bne L6      ; It was a dual button press
    ; It was a single button press
    bit INPT4
    bmi L6
    iny         ; .......1
    lda #PB2    ; Joystick 0 is a single button unit
    clc
    adc SWCHB
    sta SWCHB   ; Cut power from the dual button circuit
L6: tya         ; ......21
    rts

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

