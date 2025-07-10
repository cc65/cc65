;
; PTV-4 Player joystick driver for the C128
;
; Ullrich von Bassewitz, 2003-09-28, using the C64 driver from
; Groepaz/Hitmen, 2002-12-23, which is
; obviously based on Ullrichs driver :)
;

        .include "zeropage.inc"

        .include "joy-kernel.inc"
        .include "joy-error.inc"
        .include "c128.inc"

        .macpack generic
        .macpack module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_ptvjoy_joy

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

JOY_COUNT       = 4             ; Number of joysticks we support


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
        cli
        and     #$1F
        eor     #$1F
        rts

; Read joystick 2

joy2:   dex
        bne     joy3

        lda     #$E0
        ldy     #$FF
        sei
        sta     CIA1_DDRA
        lda     CIA1_PRA
        sty     CIA1_DDRA
        cli
        and     #$1F
        eor     #$1F
        rts

; Read joystick 3

joy3:
        lda     #%10000000      ; cia 2 port B Data-Direction
        sta     CIA2_DDRB       ; bit 7: out    bit 6-0: in

        dex
        bne     joy4

        lda     #$80            ; cia 2 port B read/write
        sta     CIA2_PRB        ; (output one at PB7)

        lda     CIA2_PRB        ; cia 2 port B read/write
        and     #$1f            ; get bit 4-0 (PB4-PB0)
        eor     #$1f
        rts

; Read joystick 4

joy4:
        lda     #$00            ; cia 2 port B read/write
        sta     CIA2_PRB        ; (output zero at PB7)

        lda     CIA2_PRB        ; cia 2 port B read/write
        and     #$0f            ; get bit 3-0 (PB3-PB0)
        sta     tmp1            ; joy 4 directions

        lda     CIA2_PRB        ; cia 2 port B read/write
        and     #%00100000      ; get bit 5 (PB5)
        lsr
        ora     tmp1
        eor     #$1f

        ldx #0
        rts
