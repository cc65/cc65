;
; Standard joystick driver for the Atari Lynx.
; The Lynx has two fire buttons. So it is not quite "standard".
;
; Modified by Karri Kaksonen, 2004-09-16
; Ullrich von Bassewitz, 2002-12-20
; Using code from Steve Schmidtke
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "lynx.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _lynx_stdjoy_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

joy_mask:
        .byte   $80                     ; JOY_UP
        .byte   $40                     ; JOY_DOWN
        .byte   $20                     ; JOY_LEFT
        .byte   $10                     ; JOY_RIGHT
        .byte   $01                     ; JOY_FIRE
        .byte   $02                     ; JOY_FIRE1
        .byte   $00                     ;
        .byte   $00                     ;

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ
        .addr   0                       ; IRQ entry unused

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 1             ; Number of joysticks we support


; ------------------------------------------------------------------------
; Data.


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda #<JOY_ERR_OK
        ldx #>JOY_ERR_OK
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
; READ: Read a particular joystick passed in A.

READ:
        ldx     #$00            ; Clear high byte
        lda     JOYSTICK        ; Read joystick
        and     #$F3            ; Mask relevant keys
        rts


