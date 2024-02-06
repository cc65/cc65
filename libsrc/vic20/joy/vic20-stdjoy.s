;
; Standard joystick driver for the VIC20. May be used multiple times when linked
; to the statically application.
;
; Ullrich von Bassewitz, 2002-12-20
; Using code from Steve Schmidtke
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "vic20.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _vic20_stdjoy_joy

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

JOY_COUNT       = 1             ; Number of joysticks we support


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
; The current implemenation will ignore the joystick number because we do only
; have one joystick

READ:   lda     #$7F            ; mask for VIA2 JOYBIT: sw3
        ldx     #$C3            ; mask for VIA1 JOYBITS: sw0,sw1,sw2,sw4
        sei                     ; necessary?

        ldy     VIA2_DDRB       ; remember the date of DDRB
        sta     VIA2_DDRB       ; set JOYBITS on this VIA for input
        lda     VIA2_PB         ; read JOYBIT: sw3
        sty     VIA2_DDRB       ; restore the state of DDRB
        asl                     ; Shift sw3 into carry

        ldy     VIA1_DDRA       ; remember the state of DDRA
        stx     VIA1_DDRA       ; set JOYBITS on this VIA for input
        lda     VIA1_PA1        ; read JOYBITS: sw0,sw1,sw2,sw4
        sty     VIA1_DDRA       ; restore the state of DDRA

        cli                     ; necessary?
        php                     ; Save sw3 in carry
        lsr                     ; Shift sw0,sw1,sw2,sw4 into bits 1-4
        tax                     ; Save sw0,sw1,sw2
        and     #$10            ; Extract sw4 in bit 4
        sta     tmp1            ; Save sw4 in bit 4
        txa                     ; Restore sw0,sw1,sw2
        lsr                     ; Shift sw0,sw1,sw2 into bits 0-2
        and     #$07            ; Mask bits 0-2
        plp                     ; Restore sw3 in carry
        bcc     @L0             ; Is sw3 set?
        ora     #$08            ; Yes: Add sw3 in bit 3
@L0:    ora     tmp1            ; Add sw4 in bit 4
        eor     #$1F            ; Active states are inverted

        ldx     #0
        rts
