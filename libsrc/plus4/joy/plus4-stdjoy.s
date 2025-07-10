;
; Standard joystick driver for the Plus/4 and C16.
; May be used multiple times when linked statically to an application.
;
; 2002-12-21, Ullrich von Bassewitz
; 2016-06-18, Greg King
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "plus4.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        .if .xmatch ("MODULE_LABEL", .string(MODULE_LABEL))
        module_header   _plus4_stdjoy_joy
        .else
        module_header   MODULE_LABEL
        .endif

; Driver signature

        .byte   $6A, $6F, $79           ; ASCII "joy"
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
        lda     #JOY_ERR_OK
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
; READ: Read a particular joystick passed in A.
;

READ:   ldy     #%11111011      ; Load index for joystick #1
        tax                     ; Test joystick number
        beq     @L1
        ldy     #%11111101      ; Load index for joystick #2
        ldx     #>$0000         ; (Return unsigned int)
@L1:    sei
        sty     TED_KBD         ; Read a joystick ...
        lda     TED_KBD         ; ... and some keys -- it's unavoidable
        cli
        eor     #%11111111

; The push buttons are in bits 6 and 7.  Both of them cannot be %1 together.
; Therefore, bit 6 can be merged with bit 7.

        clc
        adc     #%01000000
        rts
