;
; IJK joystick driver for the Atmos
; Can be used multiple times when statically linked to the application.
;
; 2002-12-20, Based on Ullrich von Bassewitz's code.
; 2017-11-01, Stefan Haubenthal
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atmos.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _atmos_ijk_joy

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

INSTALL:
        lda     #%11000000
        sta     VIA::DDRA
        sta     VIA::PRA
        lda     VIA::PRA
        and     #%00100000
        bne     ijkPresent
        lda     #JOY_ERR_NO_DEVICE
        .byte   $2C             ; Skip next opcode
ijkPresent:
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

READ:
        bne     right

        ; Ensure Printer Strobe is set to Output
        lda     #%10110111
        sta     VIA::DDRB
        ; Set Strobe Low
        lda     #%00000000
        sta     VIA::PRB
        ; Set Top two bits of PortA to Output and rest as Input
        lda     #%11000000
        sta     VIA::DDRA

        ; Select Left Joystick
        lda     #%01111111
        sta     VIA::PRA
        ; Read back Left Joystick state
        lda     VIA::PRA
        ; Mask out unused bits
        and     #%00011111
        ; Invert Bits
        eor     #%00011111
        ; Index table to conform to Generic Format
        tax
        lda     GenericIJKBits,X
        bne     L1

right:
        ; Select Right Joystick
        lda     #%10111111
        sta     VIA::PRA
        ; Read back Right Joystick state and rejig bits
        lda     VIA::PRA
        and     #%00011111
        eor     #%00011111
        tax
        lda     GenericIJKBits,X

        ; Restore VIA PortA state
L1:     ldx     #%11111111
        stx     VIA::DDRA
        inx     ; x=0
        rts

.rodata
GenericIJKBits:
        .byte   0,2,1,3,32,34,33,0,8,10,9,0,40,42,41,0
        .byte   16,18,17,0,48,50,49,0,0,0,0,0,0,0,0,0
