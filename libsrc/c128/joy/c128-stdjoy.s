;
; Standard joystick driver for the C128. May be used multiple times when linked
; to the statically application.

;
; Ullrich von Bassewitz, 2002-12-21
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "c128.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_stdjoy_joy

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

READ:   tax                     ; Joystick number into X
        bne     joy2

; Read joystick 1

joy1:   lda     #$7F
        sei
        sta     CIA1_PRA
        lda     CIA1_PRB
back:   cli
        and     #$1F
        eor     #$1F
        rts

; Read joystick 2

joy2:   ldx     #0
        lda     #$E0
        ldy     #$FF
        sei
        sta     CIA1_DDRA
        lda     CIA1_PRA
        sty     CIA1_DDRA
        jmp     back
