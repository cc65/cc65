;
; Standard joystick driver for the CX16.
; May be installed multiple times when statically linked to an application.
;
; 2021-04-07, Greg King
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

        .macpack        generic
        .macpack        module

        .importzp       tmp1


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cx16_std_joy

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
; Constant

JOY_COUNT       = $05           ; Number of joysticks we support

; ------------------------------------------------------------------------
; Data.


.code

; ------------------------------------------------------------------------
; INSTALL routine -- is called after the driver is loaded into memory.
; If possible, check if the hardware is present.
; Must return a JOY_ERR_xx code in .XA .

INSTALL:
        lda     #JOY_ERR_OK
        .assert JOY_ERR_OK = 0, error
        tax
;       rts                     ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine -- is called before the driver is removed from memory.
; Can do clean-up or whatever.  Shouldn't return anything.

UNINSTALL:
        rts

; ------------------------------------------------------------------------
; COUNT: Return the total number of possible joysticks, in .XA .

COUNT:  lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in .A .

READ:   cmp     #JOY_COUNT
        blt     :+
        lda     #$00
:       jsr     JOYSTICK_GET
        sta     tmp1
        txa
        bit     #%00001110      ; Is it NES or SNES controller?
        bze     nes

        asl     tmp1            ; Get SNES's B button
        ror     a               ; Put it next to the A button
        asl     tmp1            ; Drop SNES's Y button
        asl     a               ; Get back the B button
        ror     tmp1
        asl     a               ; Get SNES's A button
        ror     tmp1            ; Make byte look like NES pad

nes:    lda     tmp1            ; The controllers give zeroes for "pressed"
        eor     #%11111111      ; We want ones for "pressed"
        rts
