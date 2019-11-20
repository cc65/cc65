;
; Standard joystick driver for the CX16.
; May be installed multiple times when statically linked to the application.
;
; 2019-11-15 Greg King
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

        .macpack        generic
        .macpack        module


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

JOY_COUNT       = 2             ; Number of joysticks we support

; ------------------------------------------------------------------------
; Data.


.code

; ------------------------------------------------------------------------
; INSTALL routine -- is called after the driver is loaded into memory.
; If possible, check if the hardware is present, and determine the amount
; of memory available.
; Must return a JOY_ERR_xx code in .XA .

INSTALL:
        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK
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

READ:   php
        bit     #JOY_COUNT - $01
        sei
        bnz     pad2

; Read game pad 1

pad1:   ldy     JOY1            ; Allow JOY1 to be reread between interrupts
        sty     JOY1 + 2

        lda     JOY1 + 1
        bit     #%00001110
        bze     nes1

        asl     JOY1 + 2        ; Get SNES's B button
        ror     a               ; Put it next to the A button
        asl     JOY1 + 2        ; Drop SNES's Y button
        asl     a               ; Get back the B button
        ror     JOY1 + 2
        asl     a               ; Get SNES's A button
        ror     JOY1 + 2        ; Make byte look like NES pad

nes1:   lda     JOY1 + 2
        plp
        eor     #%11111111      ; (The controllers use negative logic)
        rts

; Read game pad 2

pad2:   ldy     JOY2
        sty     JOY2 + 2

        lda     JOY2 + 1
        bit     #%00001110
        bze     nes2

        asl     JOY2 + 2
        ror     a
        asl     JOY2 + 2
        asl     a
        ror     JOY2 + 2
        asl     a
        ror     JOY2 + 2

nes2:   lda     JOY2 + 2
        plp
        eor     #%11111111
        rts
