;
; MultiJoy joystick driver for the Atari. May be used multiple times when linked
; to the statically application.
;
; Ullrich von Bassewitz, 2002-12-21
; Stefan Haubenthal, 2009-04-10
; Using code from Carsten Strotmann and help from Christian Groessler
;

        .include        "zeropage.inc"

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "atari.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

.ifdef __ATARIXL__
        module_header   _atrxmj8_joy
.else
        module_header   _atrmj8_joy
.endif

; Driver signature

        .byte   $6A, $6F, $79           ; "joy"
        .byte   JOY_API_VERSION         ; Driver API version number

; Library reference

        .addr   $0000

; Jump table.

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   COUNT
        .addr   READJOY

; ------------------------------------------------------------------------
; Constants

JOY_COUNT       = 8             ; Number of joysticks we support


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
        lda     #$30
        sta     PACTL
        lda     #$F0
        sta     PORTA
        lda     #$34
        sta     PACTL
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

READJOY:
        and     #JOY_COUNT-1    ; fix joystick number
        tax                     ; Joystick number into X
        asl     a
        asl     a
        asl     a
        asl     a
        sta     PORTA

; Read joystick

        lda     STRIG0          ; get button
        asl     a
        asl     a
        asl     a
        asl     a
        ora     PORTA           ; add position information
        eor     #$1F
        cmp     oldval,x
        beq     :+
        sta     oldval,x
        ldx     #0
        stx     ATRACT          ; we have interaction, disable "attract mode"
:       ldx     #0              ; fix X
        rts

        .bss

oldval: .res    JOY_COUNT
