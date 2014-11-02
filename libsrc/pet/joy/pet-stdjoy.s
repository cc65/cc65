;
; Standard PET userport joystick driver for the PET
;
; Marco van den Heuvel, 2010-01-23
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "pet.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _pet_stdjoy_joy

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Button state masks (8 values)

        .byte   $01             ; JOY_UP
        .byte   $02             ; JOY_DOWN
        .byte   $04             ; JOY_LEFT
        .byte   $08             ; JOY_RIGHT
        .byte   $10             ; JOY_FIRE
        .byte   $00             ; JOY_FIRE2 unavailable
        .byte   $00             ; Future expansion
        .byte   $00             ; Future expansion

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READ
        .addr   0               ; IRQ entry unused

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

READ:
        tax                     ; Joystick number into X
        bne     joy2

; Read joystick 1

joy1:
        lda     #0
        sta     VIA_DDRA
        lda     VIA_PRA
        and     #$0f
        cmp     #$0c
        bne     @notc1
        lda     #$0f
        bne     @end1
@notc1:
        ora     #$10
@end1:
        eor     #$1f
        rts

; Read joystick 2

joy2:
        lda     #0
        sta     VIA_DDRA
        lda     VIA_PRA
        lsr
        lsr
        lsr
        lsr
        cmp     #$0c
        bne     @notc2
        lda     #$0f
        bne     @end2
@notc2:
        ora     #$10
@end2:
        eor     #$1f
        ldx     #0
        rts
