;
; P.A.S.E. joystick driver for the Atmos
; Can be used multiple times when statically linked to the application.
;
; 2002-12-20, Based on Ullrich von Bassewitz's code.
; 2009-12-21, Stefan Haubenthal
; 2013-07-15, Greg King
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atmos.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _atmos_pase_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $10                     ; JOY_UP
        .byte   $08                     ; JOY_DOWN
        .byte   $01                     ; JOY_LEFT
        .byte   $02                     ; JOY_RIGHT
        .byte   $20                     ; JOY_FIRE
        .byte   $00                     ; Future expansion
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

.bss
temp1:  .byte   $00
temp2:  .byte   $00

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #JOY_ERR_OK
        ldx     #0
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
        lda     #JOY_COUNT
        ldx     #0
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;

READ:
        tay

        lda     VIA::PRA
        pha
        lda     VIA::DDRA
        pha
        lda     #%11000000
        sta     VIA::DDRA
        lda     #%10000000
        sta     VIA::PRA2
        lda     VIA::PRA2
        sta     temp1
        lda     #%01000000
        sta     VIA::PRA2
        lda     VIA::PRA
        sta     temp2
        pla
        sta     VIA::DDRA
        pla
        sta     VIA::PRA2

        ldx     #0
        tya
        bne     @L1
        lda     temp1
        eor     #$FF
        rts
@L1:    lda     temp2
        eor     #$FF
        rts
