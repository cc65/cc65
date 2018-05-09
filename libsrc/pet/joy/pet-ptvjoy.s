;
; PTV-2 Player joystick driver for the PET
;
; Stefan Haubenthal, 2005-05-25
; Groepaz/Hitmen, 2002-12-23
; obviously based on Ullrichs driver :)
;

        .include "zeropage.inc"

        .include "joy-kernel.inc"
        .include "joy-error.inc"
        .include "pet.inc"

        .macpack module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _pet_ptvjoy_joy

; Driver signature

        .byte   $6A, $6F, $79   ; "joy"
        .byte   JOY_API_VERSION ; Driver API version number

; Library reference

        .addr   $0000

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 2                     ; Number of joysticks we support

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

READ:   lda     #%10000000      ; via port A Data-Direction
        sta     VIA_DDRA        ; bit 7: out    bit 6-0: in

        tax                     ; Joystick number into X
        bne     joy2

; Read joystick 1

joy1:   lda     #$80            ; via port A read/write
        sta     VIA_PA1         ; (output one at PA7)

        lda     VIA_PA1         ; via port A read/write
        and     #$1f            ; get bit 4-0 (PA4-PA0)
        eor     #$1f
        rts

; Read joystick 2

joy2:   lda     #$00            ; via port A read/write
        sta     VIA_PA1         ; (output zero at PA7)

        lda     VIA_PA1         ; via port A read/write
        and     #$0f            ; get bit 3-0 (PA3-PA0)
        sta     tmp1            ; joy 4 directions

        lda     VIA_PA1         ; via port A read/write
        and     #%00100000      ; get bit 5 (PA5)
        lsr
        ora     tmp1
        eor     #$1f

        ldx     #0
        rts
