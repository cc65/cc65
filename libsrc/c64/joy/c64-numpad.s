;
; Joystick driver using C128 number pad in 64 mode.
; May be used multiple times when linked to the statically application.
;
; Stefan Haubenthal, 2004-01-26
; Based on Ullrich von Bassewitz, 2002-12-20
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "c64.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_numpad_joy

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

JOY_COUNT       = 1             ; Number of joysticks we support


; ------------------------------------------------------------------------
; Data.

.rodata

; <U>p      '8' key
; <D>own    '2' key
; <L>eft    '4' key
; <R>ight   '6' key
; <B>utton  '5' or ENTER key

masktable:
        ; Input:  LDRBU
        ; Output: BRLDU
        .byte %00000000         ; $00
        .byte %00000001         ; $01
        .byte %00010000         ; $02
        .byte %00010001         ; $03
        .byte %00001000         ; $04
        .byte %00001001         ; $05
        .byte %00011000         ; $06
        .byte %00011001         ; $07
        .byte %00000010         ; $08
        .byte %00000011         ; $09
        .byte %00010010         ; $0A
        .byte %00010011         ; $0B
        .byte %00001010         ; $0C
        .byte %00001011         ; $0D
        .byte %00011010         ; $0E
        .byte %00011011         ; $0F
        .byte %00000100         ; $10
        .byte %00000101         ; $11
        .byte %00010100         ; $12
        .byte %00010101         ; $13
        .byte %00001100         ; $14
        .byte %00001101         ; $15
        .byte %00011100         ; $16
        .byte %00011101         ; $17
        .byte %00000110         ; $18
        .byte %00000111         ; $19
        .byte %00010110         ; $1A
        .byte %00010111         ; $1B
        .byte %00001110         ; $1C
        .byte %00001111         ; $1D
        .byte %00011110         ; $1E
        .byte %00011111         ; $1F

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #JOY_ERR_OK             ; Assume we have a "joystick"
        .assert JOY_ERR_OK = 0, error
        tax                             ; Set high byte
        ldy     VIC_CLK_128             ; Test for a C128
        cpy     #$FF
        bne     @C128                   ; Jump if we have one
        lda     #JOY_ERR_NO_DEVICE      ; No C128 -> no numpad
@C128:

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

COUNT:  lda     #JOY_COUNT
        ldx     #0
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READ:   tax                     ; Clear high byte
        lda     #$FD            ; For ENTER and '6'
        ldy     #$FE            ; For '8', '5', '2', '4'
        sei
        sta     VIC_KBD_128
        lda     CIA1_PRB
        and     #%00110000
        eor     #%00110000
        lsr                     ; Map ENTER ...
        lsr                     ; ... onto '5'
        sty     VIC_KBD_128
        eor     CIA1_PRB
        iny
        sty     VIC_KBD_128     ; Reset to $FF
        cli
        and     #%00111110
        eor     #%00111110
        lsr
        tay
        lda     masktable,y     ; Convert LDRBU to BRLDU
        rts
