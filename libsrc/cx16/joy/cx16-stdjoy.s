;
; Standard joystick driver for the CX16.
; May be used multiple times when statically linked to the application.
;
; 2019-09-23, Greg King
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _cx16_stdjoy_joy

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
; INSTALL routine. Is called after the driver is loaded into memory.
; If possible, check if the hardware is present, and determine the amount
; of memory available.
; Must return a JOY_ERR_xx code in a/x.
;

INSTALL:
        lda     #<JOY_ERR_OK
        ldx     #>JOY_ERR_OK
;       rts                     ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do clean-up or whatever. Must not return anything.
;

UNINSTALL:
        rts

; ------------------------------------------------------------------------
; COUNT: Return the total number of possible joysticks in a/x.
;

COUNT:  lda     #<JOY_COUNT
        ldx     #>JOY_COUNT
        rts

; ------------------------------------------------------------------------
; READ: Read a particular joystick passed in A.
;
; TODO: Find a way to report the SNES controller's extra four lines.
;

READ:   pha
        jsr     GETJOY
        pla
        bne     pad2

; Read game pad 1

pad1:   lda     JOY1 + 1
        bit     #%00001110
        beq     nes1
        asl     JOY1            ; Get SNES's B button
        ror     a               ; Put it next to the A button
        asl     JOY1            ; Drop SNES's Y button
        asl     a               ; Get the B button
        ror     JOY1
        asl     a               ; Get SNES's A button
        ror     JOY1            ; Make byte look like NES pad
nes1:   lda     JOY1
        eor     #%11111111      ; We don't want the pad's negative logic
        rts

; Read game pad 2

pad2:   lda     JOY2 + 1
        bit     #%00001110
        beq     nes2
        asl     JOY2
        ror     a
        asl     JOY2
        asl     a
        ror     JOY2
        asl     a
        ror     JOY2
nes2:   lda     JOY2
        eor     #%11111111
        rts
