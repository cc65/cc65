;
; Standard joystick driver for the Plus/4. May be used multiple times when linked
; to the statically application.
;
; Ullrich von Bassewitz, 2002-12-21
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "plus4.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        .if .xmatch ("MODULE_LABEL", .string(MODULE_LABEL))
        module_header   _plus4_stdjoy_joy
        .else
        module_header   MODULE_LABEL
        .endif

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $01                     ; JOY_UP
        .byte   $02                     ; JOY_DOWN
        .byte   $04                     ; JOY_LEFT
        .byte   $08                     ; JOY_RIGHT
        .byte   $10                     ; JOY_FIRE
        .byte   $00                     ; JOY_FIRE2 unavailable
        .byte   $00                     ; Future expansion
        .byte   $00                     ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ
        .addr   0                       ; IRQ entry unused

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 2             ; Number of joysticks we support


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
        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK
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
;

READ:   ldy     #$FA            ; Load index for joystick #1
        tax                     ; Test joystick number
        beq     @L1
        ldy     #$FB            ; Load index for joystick #2
@L1:    sei
        sty     TED_KBD
        lda     TED_KBD
        cli
        ldx     #$00            ; Clear high byte
        and     #$1F
        eor     #$1F
        rts

