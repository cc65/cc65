;
; Telestrat joystick driver
;
; 2002-12-20, Based on Ullrich von Bassewitz's code.
; 2017-11-01, Stefan Haubenthal
; 2020-05-20, Jede
;

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"
        .include        "telestrat.inc"

        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _telestrat_joy

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
        sta     VIA2::DDRB
        sta     VIA2::PRB
        ; We could detect joysticks because with previous command bit0,1,2,3,4 should be set to 1 after
        ; But if some one press fire or press direction, we could reach others values which could break joystick detection.
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
; How telestrat joysticks works
; PB7 and PB6 select right or left port
; When PB7 and PB6 are high, it controls two CA3083 (2 NPN transistors array) bases.
; In that case, PB0 to PB4 are set to high (it means no action are pressed)
; When the user press something then bit will be set to 0.
; Bit 0 is right
; Bit 1 is left
; Bit 2 is fire
; ...

READ:
        beq     right

        lda     VIA2::PRB
        and     #%01111111
        ora     #%01000000
        sta     VIA2::PRB
        ; then read
        lda     VIA2::PRB
        eor     #%01011111

        rts
right:
        lda     VIA2::PRB
        and     #%10111111
        ora     #%10000000
        sta     VIA2::PRB

        ; then read
        lda     VIA2::PRB
        eor     #%10011111

        rts
