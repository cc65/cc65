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

; Button state masks (8 values)

        .byte   $02                     ; JOY_UP
        .byte   $04                     ; JOY_DOWN
        .byte   $08                     ; JOY_LEFT
        .byte   $80                     ; JOY_RIGHT
        .byte   $10                     ; JOY_FIRE
        .byte   $00                     ; JOY_FIRE2 unavailable
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
; The current implemenation will ignore the joystick number because we do only
; have one joystick

READ:   lda     #$7F            ; mask for VIA2 JOYBIT: sw3
        ldx     #$C3            ; mask for VIA1 JOYBITS: sw0,sw1,sw2,sw4
        sei                     ; necessary?

        ldy     VIA2_DDRB       ; remember the date of DDRB
        sta     VIA2_DDRB       ; set JOYBITS on this VIA for input
        lda     VIA2_JOY        ; read JOYBIT: sw3
        sty     VIA2_DDRB       ; restore the state of DDRB
        asl                     ; Shift sw3 into carry

        ldy     VIA1_DDRA       ; remember the state of DDRA
        stx     VIA1_DDRA       ; set JOYBITS on this VIA for input
        lda     VIA1_JOY        ; read JOYBITS: sw0,sw1,sw2,sw4
        sty     VIA1_DDRA       ; restore the state of DDRA

        cli                     ; necessary?
        ror                     ; Shift sw3 into bit 7
        and     #$9E            ; Mask relevant bits
        eor     #$9E            ; Active states are inverted

        rts


